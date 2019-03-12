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
