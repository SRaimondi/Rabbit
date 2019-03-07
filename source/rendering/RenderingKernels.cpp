//
// Created by Simon on 2019-03-06.
//

#include "RenderingKernels.hpp"
#include "CLError.hpp"
#include "FileIO.hpp"

#include <iostream>
#include <memory>

namespace Rendering
{
namespace CL
{

RenderingKernels::RenderingKernels(cl_context context, cl_device_id device, const std::string& kernel_filename,
                                   const RenderingData& rendering_data,
                                   const SceneDescription& scene_description, const ::CL::Scene& scene)
    : initialise_kernel{ nullptr }, intersect_kernel{ nullptr },
      update_radiance_kernel{ nullptr }, deposit_samples_kernel{ nullptr }
{
    try
    {
        // Build kernel
        cl_program kernel_program{ BuildProgram(context, device, kernel_filename) };

        // Get kernels from program
        SetupKernels(kernel_program);

        // Set arguments from the given rendering data
        SetKernelArgs(rendering_data, scene_description, scene);
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
    intersect_kernel = clCreateKernel(kernel_program, "Intersect", &err_code);
    CL_CHECK_STATUS(err_code);
    update_radiance_kernel = clCreateKernel(kernel_program, "UpdateRadiance", &err_code);
    CL_CHECK_STATUS(err_code);
    deposit_samples_kernel = clCreateKernel(kernel_program, "DepositSamples", &err_code);
    CL_CHECK_STATUS(err_code);
}

void RenderingKernels::SetKernelArgs(const RenderingData& rendering_data,
                                     const SceneDescription& scene_description, const ::CL::Scene& scene)
{
    // Setup Initialise kernel
    SetInitialiseKernelArgs(rendering_data, scene_description, scene);

    // Setup Intersect kernel
    SetIntersectKernelArgs(rendering_data, scene_description, scene);
}

void RenderingKernels::SetInitialiseKernelArgs(const RenderingData& rendering_data,
                                               const SceneDescription& scene_description, const ::CL::Scene& scene)
{
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 0, sizeof(cl_mem), &scene.d_camera));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 1, sizeof(cl_mem), &rendering_data.d_rays.origin_x));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 2, sizeof(cl_mem), &rendering_data.d_rays.origin_y));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 3, sizeof(cl_mem), &rendering_data.d_rays.origin_z));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 4, sizeof(cl_mem), &rendering_data.d_rays.direction_x));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 5, sizeof(cl_mem), &rendering_data.d_rays.direction_y));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 6, sizeof(cl_mem), &rendering_data.d_rays.direction_z));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 7, sizeof(cl_mem), &rendering_data.d_rays.depth));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 8, sizeof(cl_mem),
                                 &rendering_data.d_intersections.primitive_index));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 9, sizeof(cl_mem), &rendering_data.d_samples.Li_r));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 10, sizeof(cl_mem), &rendering_data.d_samples.Li_g));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 11, sizeof(cl_mem), &rendering_data.d_samples.Li_b));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 12, sizeof(cl_mem), &rendering_data.d_samples.beta_r));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 13, sizeof(cl_mem), &rendering_data.d_samples.beta_g));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 14, sizeof(cl_mem), &rendering_data.d_samples.beta_b));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 15, sizeof(cl_mem), &rendering_data.d_samples.pixel_x));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 16, sizeof(cl_mem), &rendering_data.d_samples.pixel_y));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 17, sizeof(cl_mem), &rendering_data.d_samples.sample_offset_x));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 18, sizeof(cl_mem), &rendering_data.d_samples.sample_offset_y));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 19, sizeof(cl_mem), &rendering_data.d_pixels.pixel_r));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 20, sizeof(cl_mem), &rendering_data.d_pixels.pixel_g));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 21, sizeof(cl_mem), &rendering_data.d_pixels.pixel_b));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 22, sizeof(cl_mem), &rendering_data.d_pixels.filter_weight));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 23, sizeof(cl_mem), &rendering_data.d_xorshift_state.state));
    // We don't set argument 24 and 25, they are set when we render a specific tile
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 26, sizeof(unsigned int), &scene_description.tile_width));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 27, sizeof(unsigned int), &scene_description.tile_height));
    CL_CHECK_CALL(clSetKernelArg(initialise_kernel, 28, sizeof(unsigned int), &scene_description.pixel_samples));
}

void RenderingKernels::SetIntersectKernelArgs(const RenderingData& rendering_data,
                                              const SceneDescription& scene_description, const ::CL::Scene& scene)
{
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 0, sizeof(cl_mem), &scene.d_spheres));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 1, sizeof(unsigned int), &scene.num_spheres));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 2, sizeof(cl_mem), &rendering_data.d_rays.origin_x));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 3, sizeof(cl_mem), &rendering_data.d_rays.origin_y));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 4, sizeof(cl_mem), &rendering_data.d_rays.origin_z));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 5, sizeof(cl_mem), &rendering_data.d_rays.direction_x));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 6, sizeof(cl_mem), &rendering_data.d_rays.direction_y));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 7, sizeof(cl_mem), &rendering_data.d_rays.direction_z));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 8, sizeof(cl_mem), &rendering_data.d_intersections.hit_point_x));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 9, sizeof(cl_mem), &rendering_data.d_intersections.hit_point_y));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 10, sizeof(cl_mem), &rendering_data.d_intersections.hit_point_z));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 11, sizeof(cl_mem), &rendering_data.d_intersections.normal_x));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 12, sizeof(cl_mem), &rendering_data.d_intersections.normal_y));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 13, sizeof(cl_mem), &rendering_data.d_intersections.normal_z));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 14, sizeof(cl_mem), &rendering_data.d_intersections.uv_s));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 15, sizeof(cl_mem), &rendering_data.d_intersections.uv_t));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 16, sizeof(cl_mem), &rendering_data.d_intersections.primitive_index));
    // We don't set argument 17 and 18, they are set when we render the specific tile
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 19, sizeof(unsigned int), &scene_description.tile_width));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 20, sizeof(unsigned int), &scene_description.tile_height));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 21, sizeof(unsigned int), &scene_description.pixel_samples));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 22, sizeof(unsigned int), &scene_description.image_width));
    CL_CHECK_CALL(clSetKernelArg(intersect_kernel, 23, sizeof(unsigned int), &scene_description.image_height));
}

void RenderingKernels::Cleanup() noexcept
{
    try
    {
        if (initialise_kernel != nullptr)
        {
            CL_CHECK_CALL(clReleaseKernel(initialise_kernel));
        }
        if (intersect_kernel != nullptr)
        {
            CL_CHECK_CALL(clReleaseKernel(intersect_kernel));
        }
        if (deposit_samples_kernel != nullptr)
        {
            CL_CHECK_CALL(clReleaseKernel(deposit_samples_kernel));
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
