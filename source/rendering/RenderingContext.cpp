//
// Created by Simon on 2019-03-01.
//

#include "RenderingContext.hpp"
#include "CLError.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_writer.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <array>

namespace Rendering
{
namespace CL
{

RenderingContext::RenderingContext(cl_context context, cl_device_id device,
                                   const SceneDescription& scene_description, const ::CL::Scene& scene)
    : target_context{ context }, target_device{ device },
      output_image_width{ scene_description.image_width }, output_image_height{ scene_description.image_height },
      tile_rendering_context{ context, device, CL_QUEUE_PROFILING_ENABLE, scene_description, scene }
{
    try
    {
        // Increase reference count on context and device
        CL_CHECK_CALL(clRetainContext(target_context));
        CL_CHECK_CALL(clRetainDevice(target_device));
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

RenderingContext::~RenderingContext() noexcept
{
    Cleanup();
}

void RenderingContext::Render(const std::string& filename) const
{
    // Request tile renderer to render image
    tile_rendering_context.Render();

    // Create final image after render process
    CreateImage(filename);
}

void RenderingContext::Cleanup() noexcept
{
    try
    {
        CL_CHECK_CALL(clReleaseDevice(target_device));
        CL_CHECK_CALL(clReleaseContext(target_context));
    }
    catch (const std::exception& ex)
    {
        // TODO operator<< could throw
        std::cerr << ex.what() << std::endl;
    }
}

void RenderingContext::CreateImage(const std::string& filename) const
{
    cl_int err_code{ CL_SUCCESS };
    const size_t buffer_size{ output_image_height * output_image_width * sizeof(cl_float) };

    auto pixel_r = static_cast<float*>(clEnqueueMapBuffer(tile_rendering_context.command_queue,
                                                          tile_rendering_context.rendering_data.d_pixels.pixel_r,
                                                          CL_MAP_READ,
                                                          CL_TRUE,
                                                          0,
                                                          buffer_size,
                                                          0,
                                                          nullptr,
                                                          nullptr,
                                                          &err_code));
    CL_CHECK_STATUS(err_code);

    auto pixel_g = static_cast<float*>(clEnqueueMapBuffer(tile_rendering_context.command_queue,
                                                          tile_rendering_context.rendering_data.d_pixels.pixel_g,
                                                          CL_MAP_READ,
                                                          CL_TRUE,
                                                          0,
                                                          buffer_size,
                                                          0,
                                                          nullptr,
                                                          nullptr,
                                                          &err_code));
    CL_CHECK_STATUS(err_code);

    auto pixel_b = static_cast<float*>(clEnqueueMapBuffer(tile_rendering_context.command_queue,
                                                          tile_rendering_context.rendering_data.d_pixels.pixel_b,
                                                          CL_MAP_READ,
                                                          CL_TRUE,
                                                          0,
                                                          buffer_size,
                                                          0,
                                                          nullptr,
                                                          nullptr,
                                                          &err_code));
    CL_CHECK_STATUS(err_code);

    auto filter_weight = static_cast<float*>(clEnqueueMapBuffer(tile_rendering_context.command_queue,
                                                                tile_rendering_context.rendering_data.d_pixels.filter_weight,
                                                                CL_MAP_READ,
                                                                CL_TRUE,
                                                                0,
                                                                buffer_size,
                                                                0,
                                                                nullptr,
                                                                nullptr,
                                                                &err_code));
    CL_CHECK_STATUS(err_code);

    // Convert data to format for stbi image write
    std::vector<unsigned char> uchar_raster(3 * output_image_width * output_image_height, 0);
    for (unsigned int i = 0; i != uchar_raster.size() / 3; i++)
    {
        const float inv_filter_weight{ 1.f / filter_weight[i] };
        uchar_raster[3 * i] = static_cast<unsigned char>(pixel_r[i] * inv_filter_weight * 255);
        uchar_raster[3 * i + 1] = static_cast<unsigned char>(pixel_g[i] * inv_filter_weight * 255);
        uchar_raster[3 * i + 2] = static_cast<unsigned char>(pixel_b[i] * inv_filter_weight * 255);
    }

    // Unmap buffers
    std::array<cl_event, 4> unmap_events;
    CL_CHECK_CALL(clEnqueueUnmapMemObject(tile_rendering_context.command_queue,
                                          tile_rendering_context.rendering_data.d_pixels.pixel_r,
                                          pixel_r,
                                          0,
                                          nullptr,
                                          &unmap_events[0]));
    CL_CHECK_CALL(clEnqueueUnmapMemObject(tile_rendering_context.command_queue,
                                          tile_rendering_context.rendering_data.d_pixels.pixel_g,
                                          pixel_r,
                                          0,
                                          nullptr,
                                          &unmap_events[1]));
    CL_CHECK_CALL(clEnqueueUnmapMemObject(tile_rendering_context.command_queue,
                                          tile_rendering_context.rendering_data.d_pixels.pixel_b,
                                          pixel_r,
                                          0,
                                          nullptr,
                                          &unmap_events[2]));
    CL_CHECK_CALL(clEnqueueUnmapMemObject(tile_rendering_context.command_queue,
                                          tile_rendering_context.rendering_data.d_pixels.filter_weight,
                                          pixel_r,
                                          0,
                                          nullptr,
                                          &unmap_events[3]));

    // Write image, set flip vertical axis before
    stbi_flip_vertically_on_write(1);
    if (!stbi_write_png(filename.c_str(), output_image_width, output_image_height, 3, uchar_raster.data(), 0))
    {
        throw std::runtime_error("Error creating PNG image");
    }
    stbi_flip_vertically_on_write(0);

    CL_CHECK_CALL(clWaitForEvents(4, unmap_events.data()));
}

} // CL namespace
} // Rendering namespace