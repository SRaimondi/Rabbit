//
// Created by Simon on 2019-03-06.
//

#ifndef RABBIT_RENDERINGKERNELS_HPP
#define RABBIT_RENDERINGKERNELS_HPP

#include "RenderingData.hpp"
#include "Scene.hpp"
#include "TileDescription.hpp"

#include <string>

namespace Rendering
{
namespace CL
{

// Kernel launch size description
struct KernelLaunchSize
{
    size_t global_size;
    size_t local_size;
    size_t offset;

    constexpr explicit KernelLaunchSize(size_t g_s = 1, size_t l_s = 1, size_t o = 0) noexcept
        : global_size{ g_s }, local_size{ l_s }, offset{ o } {}
};

// This class is responsible for loading the kernel from a single file
class RenderingKernels
{
public:
    RenderingKernels(cl_context context, cl_device_id device, const std::string& kernel_filename,
                     const RenderingData& rendering_data,
                     const TileDescription& tile_description, const ::CL::Scene& scene);

    ~RenderingKernels() noexcept;

    // Data initialisation
    cl_kernel initialise_kernel;
    KernelLaunchSize initialise_launch_config;

    // If a ray is done, sends it to the next sample
    cl_kernel restart_sample_kernel;
    KernelLaunchSize restart_launch_config;

    // Intersect samples ray with spheres
    cl_kernel intersect_kernel;
    KernelLaunchSize intersect_launch_config;

    // Update radiance kernel
    // cl_kernel update_radiance_kernel;

    // Deposit the sample comping radiance to the pixel it belongs to
    // cl_kernel deposit_samples_kernel;

private:
    // Load kernel program source and build program
    cl_program BuildProgram(cl_context context, cl_device_id device, const std::string& kernel_filename) const;

    // Setup kernel from program
    void SetupKernels(cl_program kernel_program);

    // Set kernel arguments
    void SetKernelArgs(const RenderingData& rendering_data,
                       const TileDescription& tile_description, const ::CL::Scene& scene);

    // Set arguments for Initialise kernel
    void SetInitialiseKernelArgs(const RenderingData& rendering_data,
                                 const TileDescription& tile_description, const ::CL::Scene& scene);

    // Set argument for Restart kernel
    void SetRestartKernelArgs(const RenderingData& rendering_data,
                              const TileDescription& tile_description, const ::CL::Scene& scene);

    // Set arguments for Intersect kernel
    void SetIntersectKernelArgs(const RenderingData& rendering_data,
                                const TileDescription& tile_description, const ::CL::Scene& scene);

    // Get preferred wg multiple size and max wg size for a kernel
    std::pair<size_t, size_t> GetWGInfo(cl_kernel kernel, cl_device_id device) const;

    // Setup kernel launch sizes
    void SetupLaunchConfig(const TileDescription& tile_description, cl_device_id device);

    // Setup launch config for Initialise kernel
    void SetupInitialiseLaunchConfig(const TileDescription& tile_description, cl_device_id device);

    // Setup launch config for Restart kernel
    void SetupRestartLaunchConfig(const TileDescription& tile_description, cl_device_id device);

    // Setup launch config for Intersect kernel
    void SetupIntersectLaunchConfig(const TileDescription& tile_description, cl_device_id device);

    // Cleanup OpenCL resource without throwing
    void Cleanup() noexcept;
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_RENDERINGKERNELS_HPP
