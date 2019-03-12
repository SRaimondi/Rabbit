//
// Created by Simon on 2019-03-01.
//

#include "TileRendering.hpp"
#include "FileIO.hpp"
#include "CLError.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_writer.hpp"

#include <iostream>

namespace Rendering
{
namespace CL
{

TileRendering::TileRendering(cl_context context, cl_device_id device, cl_command_queue_properties queue_properties,
                             const SceneDescription& scene_description, const ::CL::Scene& scene)
    : command_queue{ nullptr },
      tile_description{ scene_description.tile_width, scene_description.tile_height, scene_description.pixel_samples },
      rendering_data{ context, tile_description.TotalPixels(), tile_description.TotalSamples() },
      rendering_kernel{ context, device, "./kernel/rendering_kernel.cl", rendering_data, tile_description, scene }
{
    cl_int err_code{ CL_SUCCESS };

    try
    {
        // Create command queue
        command_queue = clCreateCommandQueue(context, device, queue_properties, &err_code);
        CL_CHECK_STATUS(err_code);
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

TileRendering::~TileRendering() noexcept
{
    Cleanup();
}

void TileRendering::Render() const
{
    // Run Initialise kernel
    cl_event initialise_event;
    rendering_kernel.RunInitialise(command_queue, 0, nullptr, &initialise_event);

    // Map memory to host and check what is inside
    cl_int err_code{ CL_SUCCESS };
    auto ray_depth = static_cast<cl_uint*>(clEnqueueMapBuffer(command_queue,
                                                              rendering_data.d_rays.depth,
                                                              CL_TRUE,
                                                              CL_MAP_READ,
                                                              0,
                                                              tile_description.TotalSamples() * sizeof(cl_uint),
                                                              1,
                                                              &initialise_event,
                                                              nullptr,
                                                              &err_code));
    CL_CHECK_STATUS(err_code);

    auto xorshift_state = static_cast<cl_uint*>(clEnqueueMapBuffer(command_queue,
                                                                   rendering_data.d_xorshift_state.state,
                                                                   CL_TRUE,
                                                                   CL_MAP_READ,
                                                                   0,
                                                                   tile_description.TotalSamples() * sizeof(cl_uint),
                                                                   1,
                                                                   &initialise_event,
                                                                   nullptr,
                                                                   &err_code));
    CL_CHECK_STATUS(err_code);

    // Run restart kernel
    cl_event restart_event;
    rendering_kernel.RunRestart(command_queue, 1, &initialise_event, &restart_event);

    // Map ray origin to debug
    auto ray_origin_x = static_cast<float*>(clEnqueueMapBuffer(command_queue,
                                                               rendering_data.d_rays.origin_x,
                                                               CL_TRUE,
                                                               CL_MAP_READ,
                                                               0,
                                                               tile_description.TotalSamples() * sizeof(cl_float),
                                                               1,
                                                               &restart_event,
                                                               nullptr,
                                                               &err_code));
    CL_CHECK_STATUS(err_code);
    auto ray_origin_y = static_cast<float*>(clEnqueueMapBuffer(command_queue,
                                                               rendering_data.d_rays.origin_y,
                                                               CL_TRUE,
                                                               CL_MAP_READ,
                                                               0,
                                                               tile_description.TotalSamples() * sizeof(cl_float),
                                                               1,
                                                               &restart_event,
                                                               nullptr,
                                                               &err_code));
    CL_CHECK_STATUS(err_code);
    auto ray_origin_z = static_cast<float*>(clEnqueueMapBuffer(command_queue,
                                                               rendering_data.d_rays.origin_z,
                                                               CL_TRUE,
                                                               CL_MAP_READ,
                                                               0,
                                                               tile_description.TotalSamples() * sizeof(cl_float),
                                                               1,
                                                               &restart_event,
                                                               nullptr,
                                                               &err_code));
    CL_CHECK_STATUS(err_code);

    // Map ray origin to debug
    auto ray_direction_x = static_cast<float*>(clEnqueueMapBuffer(command_queue,
                                                                  rendering_data.d_rays.direction_x,
                                                                  CL_TRUE,
                                                                  CL_MAP_READ,
                                                                  0,
                                                                  tile_description.TotalSamples() * sizeof(cl_float),
                                                                  1,
                                                                  &restart_event,
                                                                  nullptr,
                                                                  &err_code));
    CL_CHECK_STATUS(err_code);
    auto ray_direction_y = static_cast<float*>(clEnqueueMapBuffer(command_queue,
                                                                  rendering_data.d_rays.direction_y,
                                                                  CL_TRUE,
                                                                  CL_MAP_READ,
                                                                  0,
                                                                  tile_description.TotalSamples() * sizeof(cl_float),
                                                                  1,
                                                                  &restart_event,
                                                                  nullptr,
                                                                  &err_code));
    CL_CHECK_STATUS(err_code);
    auto ray_direction_z = static_cast<float*>(clEnqueueMapBuffer(command_queue,
                                                                  rendering_data.d_rays.direction_z,
                                                                  CL_TRUE,
                                                                  CL_MAP_READ,
                                                                  0,
                                                                  tile_description.TotalSamples() * sizeof(cl_float),
                                                                  1,
                                                                  &restart_event,
                                                                  nullptr,
                                                                  &err_code));
    CL_CHECK_STATUS(err_code);

    auto pixel_x = static_cast<cl_uint*>(clEnqueueMapBuffer(command_queue,
                                                            rendering_data.d_samples.pixel_x,
                                                            CL_TRUE,
                                                            CL_MAP_READ,
                                                            0,
                                                            tile_description.TotalSamples() * sizeof(cl_uint),
                                                            1,
                                                            &restart_event,
                                                            nullptr,
                                                            &err_code));
    CL_CHECK_STATUS(err_code);

    auto pixel_y = static_cast<cl_uint*>(clEnqueueMapBuffer(command_queue,
                                                            rendering_data.d_samples.pixel_y,
                                                            CL_TRUE,
                                                            CL_MAP_READ,
                                                            0,
                                                            tile_description.TotalSamples() * sizeof(cl_uint),
                                                            1,
                                                            &restart_event,
                                                            nullptr,
                                                            &err_code));
    CL_CHECK_STATUS(err_code);

    auto sample_x = static_cast<cl_float*>(clEnqueueMapBuffer(command_queue,
                                                              rendering_data.d_samples.sample_offset_x,
                                                              CL_TRUE,
                                                              CL_MAP_READ,
                                                              0,
                                                              tile_description.TotalSamples() * sizeof(cl_float),
                                                              1,
                                                              &restart_event,
                                                              nullptr,
                                                              &err_code));
    CL_CHECK_STATUS(err_code);

    auto sample_y = static_cast<cl_float*>(clEnqueueMapBuffer(command_queue,
                                                              rendering_data.d_samples.sample_offset_y,
                                                              CL_TRUE,
                                                              CL_MAP_READ,
                                                              0,
                                                              tile_description.TotalSamples() * sizeof(cl_float),
                                                              1,
                                                              &restart_event,
                                                              nullptr,
                                                              &err_code));
    CL_CHECK_STATUS(err_code);

    // Run Intersect kernel
    cl_event intersect_event;
    rendering_kernel.RunIntersect(command_queue, 1, &restart_event, &intersect_event);

    auto primitive_index = static_cast<cl_uint*>(clEnqueueMapBuffer(command_queue,
                                                                    rendering_data.d_intersections.primitive_index,
                                                                    CL_TRUE,
                                                                    CL_MAP_READ,
                                                                    0,
                                                                    tile_description.TotalSamples() * sizeof(cl_uint),
                                                                    1,
                                                                    &intersect_event,
                                                                    nullptr,
                                                                    &err_code));

    std::vector<unsigned char> uchar_raster(3 * tile_description.TotalPixels());
    for (unsigned int i = 0; i != tile_description.TotalSamples(); i++)
    {
        if (primitive_index[i] != std::numeric_limits<cl_uint>::max())
        {
            uchar_raster[3 * i] = 255;
            uchar_raster[3 * i + 1] = 0;
            uchar_raster[3 * i + 2] = 0;
        }
        else
        {
            uchar_raster[3 * i] = 255;
            uchar_raster[3 * i + 1] = 255;
            uchar_raster[3 * i + 2] = 255;
        }
//        uchar_raster[3 * i] = static_cast<unsigned char>(std::abs(ray_direction_x[i]) * 255);
//        uchar_raster[3 * i + 1] = static_cast<unsigned char>(std::abs(ray_direction_y[i]) * 255);
//        uchar_raster[3 * i + 2] = static_cast<unsigned char>(std::abs(ray_direction_z[i]) * 255);
    }

    // Write image, set flip vertical axis before
    stbi_flip_vertically_on_write(1);
    if (!stbi_write_png("DaiCazzo.png", tile_description.tile_width, tile_description.tile_height, 3,
                        uchar_raster.data(), 0))
    {
        throw std::runtime_error("Error creating PNG image");
    }
    stbi_flip_vertically_on_write(0);
}

void TileRendering::Cleanup() noexcept
{
    try
    {
        if (command_queue != nullptr)
        {
            CL_CHECK_CALL(clReleaseCommandQueue(command_queue));
        }
    }
    catch (const std::exception& ex)
    {
        // TODO operator<< could throw
        std::cerr << ex.what() << std::endl;
    }
}

} // CL namespace
} // Rendering namespace
