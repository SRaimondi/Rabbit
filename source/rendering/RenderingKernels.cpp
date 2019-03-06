//
// Created by Simon on 2019-03-06.
//

#include "RenderingKernels.hpp"
#include "CLError.hpp"
#include "FileIO.hpp"

#include <iostream>

namespace Rendering
{
namespace CL
{

RenderingKernels::RenderingKernels(cl_context context, cl_device_id device,
                                   const std::string& kernel_filename)
    : initialisation_kernel{ nullptr }, setup_samples_kernel{ nullptr },
      intersect_kernel{ nullptr }, deposit_samples_kernel{ nullptr }
{
    try
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
        err_code = clBuildProgram(kernel_program, 1, &device, program_options.c_str(), nullptr, nullptr));
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
            throw std::runtime_error(std::string(program_build_log.get()));
        }

        // Get kernels from program
        
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

void RenderingKernels::Cleanup() noexcept
{
    try
    {
        if (initialisation_kernel != nullptr)
        {
            CL_CHECK_CALL(clReleaseKernel(initialisation_kernel));
        }
        if (setup_samples_kernel != nullptr)
        {
            CL_CHECK_CALL(clReleaseKernel(setup_samples_kernel));
        }
        if (setup_samples_kernel != nullptr)
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
