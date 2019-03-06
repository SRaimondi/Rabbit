//
// Created by Simon on 2019-03-01.
//

#include "TileRendering.hpp"
#include "FileIO.hpp"
#include "CLError.hpp"

#include <iostream>

namespace Rendering
{
namespace CL
{

TileRenderingData::TileRenderingData(cl_context context, const TileDescription& tile_description)
    : d_rays{ context, tile_description.TotalSamples() },
      d_intersections{ context, tile_description.TotalSamples() },
      d_samples{ context, tile_description.TotalSamples() },
      d_pixels{ context, tile_description.TotalPixels() },
      d_xorshift_state{ context, tile_description.TotalSamples() }
{}

TileRendering::TileRendering(cl_context context, cl_device_id device,
                             cl_command_queue_properties queue_properties,
                             const SceneDescription& scene_description,
                             const Camera& camera)
    : command_queue{ nullptr },
      tile_description{ scene_description.tile_width, scene_description.tile_height, scene_description.pixel_samples },
      rendering_data{ context, tile_description },
      num_spheres{ static_cast<unsigned int>(scene_description.loaded_spheres.size()) }, d_spheres{ nullptr },
      d_camera{ nullptr }
{
    cl_int err_code{ CL_SUCCESS };

    try
    {
        // Create command queue
        command_queue = clCreateCommandQueue(context, device, queue_properties, &err_code);
        CL_CHECK_STATUS(err_code);

        // Create spheres buffer
        d_spheres = clCreateBuffer(context, CL_MEM_READ_ONLY, num_spheres * sizeof(Sphere), nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        // Copy data to device
        CL_CHECK_CALL(clEnqueueWriteBuffer(command_queue, d_spheres, CL_TRUE, 0, num_spheres * sizeof(Sphere),
                                           scene_description.loaded_spheres.data(), 0, nullptr, nullptr));

        // Create camera buffer
        d_camera = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(Camera), nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        // Copy data to device
        CL_CHECK_CALL(clEnqueueWriteBuffer(command_queue, d_camera, CL_TRUE, 0, sizeof(Camera),
                                           &camera, 0, nullptr, nullptr));
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

void TileRendering::Cleanup() noexcept
{
    try
    {
        if (command_queue != nullptr)
        {
            CL_CHECK_CALL(clReleaseCommandQueue(command_queue));
        }
        if (d_spheres != nullptr)
        {
            CL_CHECK_CALL(clReleaseMemObject(d_spheres));
        }
        if (d_camera != nullptr)
        {
            CL_CHECK_CALL(clReleaseMemObject(d_camera));
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
