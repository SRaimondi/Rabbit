//
// Created by Simon on 2019-03-06.
//

#ifndef RABBIT_RENDERINGKERNELS_HPP
#define RABBIT_RENDERINGKERNELS_HPP

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <string>

namespace Rendering
{
namespace CL
{

// This class is responsible for loading the kernel from a single file
class RenderingKernels
{
public:
    RenderingKernels(cl_context context, cl_device_id device, const std::string& kernel_filename);

    ~RenderingKernels() noexcept;

    // Data initialisation
    cl_kernel initialisation_kernel;

    // Intersect samples ray with spheres
    cl_kernel intersect_kernel;

    // Deposit the sample comping radiance to the pixel it belongs to
    cl_kernel deposit_samples_kernel;

private:
    // Load kernel program source and build program
    cl_program BuildProgram(cl_context context, cl_device_id device, const std::string& kernel_filename) const;

    // Cleanup OpenCL resource without throwing
    void Cleanup() noexcept;
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_RENDERINGKERNELS_HPP
