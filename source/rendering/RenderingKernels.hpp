//
// Created by Simon on 2019-03-06.
//

#ifndef RABBIT_RENDERINGKERNELS_HPP
#define RABBIT_RENDERINGKERNELS_HPP

#include "RenderingData.hpp"
#include "Scene.hpp"

#include <string>

namespace Rendering
{
namespace CL
{

// This class is responsible for loading the kernel from a single file
class RenderingKernels
{
public:
    RenderingKernels(cl_context context, cl_device_id device, const std::string& kernel_filename,
                     const RenderingData& rendering_data,
                     const SceneDescription& scene_description, const ::CL::Scene& scene);

    ~RenderingKernels() noexcept;

    // Data initialisation
    cl_kernel initialise_kernel;

    // Intersect samples ray with spheres
    cl_kernel intersect_kernel;

    // Update radiance kernel
    cl_kernel update_radiance_kernel;

    // Deposit the sample comping radiance to the pixel it belongs to
    cl_kernel deposit_samples_kernel;

private:
    // Load kernel program source and build program
    cl_program BuildProgram(cl_context context, cl_device_id device, const std::string& kernel_filename) const;

    // Setup kernel from program
    void SetupKernels(cl_program kernel_program);

    // Set kernel arguments
    void SetKernelArguments(const RenderingData& rendering_data,
                            const SceneDescription& scene_description, const ::CL::Scene& scene);

    // Set argument for intialisation kernel
    void SetInitialiseKernelArgs(const RenderingData& rendering_data,
                                 const SceneDescription& scene_description, const ::CL::Scene& scene);

    // Cleanup OpenCL resource without throwing
    void Cleanup() noexcept;
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_RENDERINGKERNELS_HPP
