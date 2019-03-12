//
// Created by Simon on 2019-03-06.
//

#include "RenderingKernels.hpp"
#include "CLError.hpp"
#include "FileIO.hpp"
#include "Common.hpp"

#include <iostream>
#include <memory>

namespace Rendering
{
namespace CL
{

RenderingKernels::RenderingKernels(cl_context context, cl_device_id device, const std::string& kernel_filename,
                                   const RenderingData& rendering_data,
                                   const TileDescription& tile_description, const ::CL::Scene& scene)
    : initialise_kernel{ nullptr }, restart_sample_kernel{ nullptr }, intersect_kernel{ nullptr } /*,
      update_radiance_kernel{ nullptr }, deposit_samples_kernel{ nullptr }*/
{
    try
    {
        // Build kernel
        cl_program kernel_program{ BuildProgram(context, device, kernel_filename) };

        // Get kernels from program
        SetupKernels(kernel_program);

        // Release program
        CL_CHECK_CALL(clReleaseProgram(kernel_program));

        // Set arguments from the given rendering data
        SetKernelArgs(rendering_data, tile_description, scene);

        // Compute the sizes for launching the kernels
        SetupLaunchConfig(tile_description, device);
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

RenderingKernels::~RenderingKernels() noexcept
{
    Cleanup();
}

void RenderingKernels::RunInitialise(cl_command_queue queue, cl_uint num_wait_events, const cl_event* wait_events,
                                     cl_event* kernel_event) const
{
    CL_CHECK_CALL(clEnqueueNDRangeKernel(queue,
                                         initialise_kernel,
                                         1,
                                         &initialise_launch_config.offset,
                                         &initialise_launch_config.global_size,
                                         &initialise_launch_config.local_size,
                                         num_wait_events,
                                         wait_events,
                                         kernel_event));
}

void RenderingKernels::RunRestart(cl_command_queue queue, cl_uint num_wait_events, const cl_event* wait_events,
                                  cl_event* kernel_event) const
{
    CL_CHECK_CALL(clEnqueueNDRangeKernel(queue,
                                         restart_sample_kernel,
                                         1,
                                         &restart_launch_config.offset,
                                         &restart_launch_config.global_size,
                                         &restart_launch_config.local_size,
                                         num_wait_events,
                                         wait_events,
                                         kernel_event));
}

void RenderingKernels::RunIntersect(cl_command_queue queue, cl_uint num_wait_events, const cl_event* wait_events,
                                    cl_event* kernel_event) const
{
    CL_CHECK_CALL(clEnqueueNDRangeKernel(queue,
                                         intersect_kernel,
                                         1,
                                         &intersect_launch_config.offset,
                                         &intersect_launch_config.global_size,
                                         &intersect_launch_config.local_size,
                                         num_wait_events,
                                         wait_events,
                                         kernel_event));
}

cl_program RenderingKernels::BuildProgram(cl_context context, cl_device_id device,
                                          const std::string& kernel_filename) const
{
    // Read file source
    const auto kernel_source{ IO::ReadFile(kernel_filename) };
    const char* c_ptr_source{ kernel_source.c_str() };

    // Create program
    cl_int err_code{ CL_SUCCESS };
    cl_program kernel_program{ clCreateProgramWithSource(context, 1, &c_ptr_source, nullptr, &err_code) };
    CL_CHECK_STATUS(err_code);

    // Build program
    const std::string program_options{ "-cl-std=CL1.2 -cl-mad-enable -cl-no-signed-zeros" };
    err_code = clBuildProgram(kernel_program, 1, &device, program_options.c_str(), nullptr, nullptr);
    if (err_code != CL_SUCCESS)
    {
        // We have an error in the program build
        size_t program_build_log_size;
        CL_CHECK_CALL(clGetProgramBuildInfo(kernel_program, device, CL_PROGRAM_BUILD_LOG,
                                            0, nullptr, &program_build_log_size));
        auto program_build_log{ std::make_unique<char[]>(program_build_log_size) };
        CL_CHECK_CALL(clGetProgramBuildInfo(kernel_program, device, CL_PROGRAM_BUILD_LOG,
                                            program_build_log_size, program_build_log.get(), nullptr));

        // Release program and throw exception with log
        CL_CHECK_CALL(clReleaseProgram(kernel_program));
        throw std::runtime_error{ program_build_log.get() };
    }

    return kernel_program;
}

void RenderingKernels::SetupKernels(cl_program kernel_program)
{
    cl_int err_code{ CL_SUCCESS };
    initialise_kernel = clCreateKernel(kernel_program, "Initialise", &err_code);
    CL_CHECK_STATUS(err_code);
    restart_sample_kernel = clCreateKernel(kernel_program, "RestartSample", &err_code);
    CL_CHECK_STATUS(err_code);
    intersect_kernel = clCreateKernel(kernel_program, "Intersect", &err_code);
    CL_CHECK_STATUS(err_code);
//    update_radiance_kernel = clCreateKernel(kernel_program, "UpdateRadiance", &err_code);
//    CL_CHECK_STATUS(err_code);
//    deposit_samples_kernel = clCreateKernel(kernel_program, "DepositSamples", &err_code);
//    CL_CHECK_STATUS(err_code);
}

void RenderingKernels::SetKernelArgs(const RenderingData& rendering_data,
                                     const TileDescription& tile_description, const ::CL::Scene& scene)
{
    SetInitialiseKernelArgs(rendering_data, tile_description, scene);
    SetRestartKernelArgs(rendering_data, tile_description, scene);
    SetIntersectKernelArgs(rendering_data, tile_description, scene);
}

void RenderingKernels::SetInitialiseKernelArgs(const RenderingData& rendering_data,
                                               const TileDescription& tile_description, const ::CL::Scene& scene)
{
    cl_uint arg_index{ 0 };
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.depth));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_xorshift_state.state));
    const auto total_samples = static_cast<cl_uint>(tile_description.TotalSamples());
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, arg_index++, sizeof(unsigned int), &total_samples));
}

void RenderingKernels::SetRestartKernelArgs(const RenderingData& rendering_data,
                                            const TileDescription& tile_description, const ::CL::Scene& scene)
{
    cl_uint arg_index{ 0 };
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &scene.d_camera));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.origin_x));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.origin_y));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.origin_z));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_rays.direction_x));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_rays.direction_y));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_rays.direction_z));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.depth));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_intersections.primitive_index));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_samples.Li_r));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_samples.Li_g));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_samples.Li_b));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_samples.beta_r));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_samples.beta_g));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_samples.beta_b));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_samples.pixel_x));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_samples.pixel_y));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_samples.sample_offset_x));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_samples.sample_offset_y));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_pixels.pixel_r));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_pixels.pixel_g));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_pixels.pixel_b));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_pixels.filter_weight));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_xorshift_state.state));
    const cl_uint tile_width = tile_description.Width();
    const cl_uint tile_height = tile_description.Height();
    const cl_uint pixel_samples = tile_description.PixelSamples();
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(unsigned int),
                                 &tile_width));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(unsigned int),
                                 &tile_height));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(unsigned int),
                                 &pixel_samples));
    CL_CHECK_CALL(clSetKernelArg(restart_sample_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_samples.samples_done));
}

void RenderingKernels::SetIntersectKernelArgs(const RenderingData& rendering_data,
                                              const TileDescription& tile_description, const ::CL::Scene& scene)
{
    cl_uint arg_index{ 0 };
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &scene.d_spheres));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_uint), &scene.num_spheres));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.origin_x));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.origin_y));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.origin_z));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.direction_x));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.direction_y));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.direction_z));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_rays.depth));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_intersections.hit_point_x));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_intersections.hit_point_y));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_intersections.hit_point_z));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_intersections.normal_x));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_intersections.normal_y));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_intersections.normal_z));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_intersections.uv_s));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem), &rendering_data.d_intersections.uv_t));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_mem),
                                 &rendering_data.d_intersections.primitive_index));
    const auto total_samples = tile_description.TotalSamples();
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, arg_index++, sizeof(cl_uint), &total_samples));
}

std::pair<size_t, size_t> RenderingKernels::GetWGInfo(cl_kernel kernel, cl_device_id device) const
{
    // Get the preferred multiple size multiple for the device
    size_t preferred_wg_size_multiple{ 0 };
    CL_CHECK_CALL(clGetKernelWorkGroupInfo(initialise_kernel, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                                           sizeof(size_t), &preferred_wg_size_multiple, nullptr));

    // Get the maximum size we can use as work-group size for our kernel
    size_t max_wg_size{ 0 };
    CL_CHECK_CALL(clGetKernelWorkGroupInfo(initialise_kernel, device, CL_KERNEL_WORK_GROUP_SIZE,
                                           sizeof(size_t), &max_wg_size, nullptr));

    return { preferred_wg_size_multiple, max_wg_size };
}

void RenderingKernels::SetupLaunchConfig(const TileDescription& tile_description, cl_device_id device)
{
    SetupInitialiseLaunchConfig(tile_description, device);
    SetupRestartLaunchConfig(tile_description, device);
    SetupIntersectLaunchConfig(tile_description, device);
}

void RenderingKernels::SetupInitialiseLaunchConfig(const TileDescription& tile_description, cl_device_id device)
{
    // Get the preferred sizes for the kernel
    const auto wg_info = GetWGInfo(initialise_kernel, device);
    // Compute size for the kernel
    initialise_launch_config.local_size = RoundDown(wg_info.second, wg_info.first);
    initialise_launch_config.global_size = RoundUp(static_cast<size_t>(tile_description.TotalSamples()),
                                                   initialise_launch_config.local_size);
}

void RenderingKernels::SetupRestartLaunchConfig(const TileDescription& tile_description, cl_device_id device)
{
    // Get the preferred sizes for the kernel
    const auto wg_info = GetWGInfo(initialise_kernel, device);
    // Compute size for the kernel
    restart_launch_config.local_size = RoundDown(wg_info.second, wg_info.first);
    restart_launch_config.global_size = RoundUp(static_cast<size_t>(tile_description.TotalSamples()),
                                                restart_launch_config.local_size);
}

void RenderingKernels::SetupIntersectLaunchConfig(const TileDescription& tile_description, cl_device_id device)
{
    // Get the preferred sizes for the kernel
    const auto wg_info = GetWGInfo(initialise_kernel, device);
    // Compute size for the kernel
    intersect_launch_config.local_size = RoundDown(wg_info.second, wg_info.first);
    intersect_launch_config.global_size = RoundUp(static_cast<size_t>(tile_description.TotalSamples()),
                                                  intersect_launch_config.local_size);
}

void RenderingKernels::Cleanup() noexcept
{
    try
    {
        if (initialise_kernel != nullptr)
        {
            CL_CHECK_CALL(clReleaseKernel(initialise_kernel));
        }
        if (restart_sample_kernel != nullptr)
        {
            CL_CHECK_CALL(clReleaseKernel(restart_sample_kernel));
        }
        if (intersect_kernel != nullptr)
        {
            CL_CHECK_CALL(clReleaseKernel(intersect_kernel));
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
