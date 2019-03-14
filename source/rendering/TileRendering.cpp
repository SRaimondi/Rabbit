//
// Created by Simon on 2019-03-01.
//

#include "TileRendering.hpp"
#include "FileIO.hpp"
#include "CLError.hpp"

#include <iostream>
#include <limits>
#include <array>

namespace Rendering
{
namespace CL
{

TileRendering::TileRendering(cl_context context, cl_device_id device, cl_command_queue_properties queue_properties,
                             const SceneDescription& scene_description, const ::CL::Scene& scene)
    : command_queue{ nullptr },
      tile_description{ scene_description.tile_width, scene_description.tile_height, scene_description.pixel_samples },
      rendering_data{ context, scene_description.image_width * scene_description.image_height,
                      tile_description.TotalSamples() },
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
    // Synchronisation events
    cl_event initialise_event, restart_event, intersect_event,
        sample_event, update_radiance_event, deposit_samples_event;

    // Initially set all pixels and filter weight to zero
    SetRasterToZero();

    // Run Initialise kernel
    rendering_kernel.RunInitialise(command_queue, 0, nullptr, &initialise_event);
    bool first_restart{ true };

    cl_uint samples_done{ 0 };
    while (true)
    {
        // Restart the samples
        if (first_restart)
        {
            rendering_kernel.RunRestart(command_queue, 1, &initialise_event, &restart_event);
            first_restart = false;
        }
        else
        {
            rendering_kernel.RunRestart(command_queue, 1, &deposit_samples_event, &restart_event);
        }

        // Copy to host the number of samples that are done
        CL_CHECK_CALL(clEnqueueReadBuffer(command_queue,
                                          rendering_data.d_samples.samples_done,
                                          CL_TRUE,
                                          0, sizeof(cl_uint), &samples_done,
                                          0, nullptr, nullptr));
        // Check if we are done rendering
        if (samples_done == tile_description.TotalSamples())
        {
            break;
        }

        // Intersect the rays
        rendering_kernel.RunIntersect(command_queue, 1, &restart_event, &intersect_event);

        // Sample the BRDF
        rendering_kernel.RunSampleBRDF(command_queue, 1, &intersect_event, &sample_event);

        // Update radiance
        rendering_kernel.RunUpdateRadiance(command_queue, 1, &sample_event, &update_radiance_event);

        // Deposit samples
        rendering_kernel.RunDepositSamples(command_queue, 1, &update_radiance_event, &deposit_samples_event);
    }
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

void TileRendering::SetRasterToZero() const
{
    std::array<cl_event, 4> fill_events;

    const cl_float zero{ 0 };
    const size_t buffer_size{ rendering_data.d_pixels.num_pixels * sizeof(cl_float) };

    CL_CHECK_CALL(clEnqueueFillBuffer(command_queue, rendering_data.d_pixels.pixel_r, &zero, sizeof(cl_float),
                                      0, buffer_size, 0, nullptr, &fill_events[0]));
    CL_CHECK_CALL(clEnqueueFillBuffer(command_queue, rendering_data.d_pixels.pixel_g, &zero, sizeof(cl_float),
                                      0, buffer_size, 0, nullptr, &fill_events[1]));
    CL_CHECK_CALL(clEnqueueFillBuffer(command_queue, rendering_data.d_pixels.pixel_b, &zero, sizeof(cl_float),
                                      0, buffer_size, 0, nullptr, &fill_events[2]));
    CL_CHECK_CALL(clEnqueueFillBuffer(command_queue, rendering_data.d_pixels.filter_weight, &zero, sizeof(cl_float),
                                      0, buffer_size, 0, nullptr, &fill_events[3]));
    CL_CHECK_CALL(clWaitForEvents(4, fill_events.data()));
}

} // CL namespace
} // Rendering namespace
