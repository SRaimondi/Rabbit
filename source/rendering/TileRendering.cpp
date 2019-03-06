//
// Created by Simon on 2019-03-01.
//

#include "TileRendering.hpp"
#include "FileIO.hpp"

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

//TileRenderingKernels::TileRenderingKernels(const cl::Context& context,
//                                           const std::pair<const std::string, const std::string>& initialisation_desc,
//                                           const std::pair<const std::string, const std::string>& restart_desc,
//                                           const std::pair<const std::string, const std::string>& intersect_desc,
//                                           const std::pair<const std::string, const std::string>& update_desc,
//                                           const std::pair<const std::string, const std::string>& deposit_desc)
//{
//    const std::string build_options{
//        "-cl-std=CL1.2 -cl-mad-enable -cl-finite-math-only  -cl-denorms-are-zero -I./kernel" };
//
//    // For each string pair, crete the program and initialise the kernel from it
//    const cl::Program init_program{ context, IO::ReadFile(initialisation_desc.first) };
//    init_program.build(build_options.c_str());
//    initialisation = cl::Kernel{ init_program, initialisation_desc.second.c_str() };
//
//    const cl::Program restart_program{ context, IO::ReadFile(restart_desc.first) };
//    restart_program.build(build_options.c_str());
//    restart_sample = cl::Kernel{ restart_program, restart_desc.second.c_str() };
//
//    const cl::Program intersect_program{ context, IO::ReadFile(intersect_desc.first) };
//    intersect_program.build(build_options.c_str());
//    intersect_scene = cl::Kernel{ intersect_program, intersect_desc.second.c_str() };
//
//    const cl::Program update_program{ context, IO::ReadFile(update_desc.first) };
//    update_program.build(build_options.c_str());
//    update_radiance = cl::Kernel{ update_program, update_desc.second.c_str() };
//
//    const cl::Program deposit_program{ context, IO::ReadFile(deposit_desc.first) };
//    deposit_program.build(build_options.c_str());
//    deposit_sample = cl::Kernel{ deposit_program, deposit_desc.second.c_str() };
//}

TileRendering::TileRendering(const cl::Context& context, const cl::Device& device,
                             cl_command_queue_properties queue_properties,
                             const SceneDescription& scene_description,
                             const Camera& camera)
    : command_queue{ context, device, queue_properties },
      tile_description{ scene_description.tile_width, scene_description.tile_height, scene_description.pixel_samples },
      rendering_data{ context, tile_description },
      num_spheres{ static_cast<unsigned int>(scene_description.loaded_spheres.size()) },
      d_spheres{ context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, num_spheres * sizeof(Sphere),
                 const_cast<Sphere*>(scene_description.loaded_spheres.data()) },
      d_camera{ context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Camera), const_cast<Camera*>(&camera) },
    // TODO This should come from some config file
      kernels{ context,
               std::make_pair("./kernel/initialisation.cl", "Initialise"),
               std::make_pair("./kernel/restart.cl", "RestartSample"),
               std::make_pair("./kernel/intersect.cl", "Intersect"),
               std::make_pair("./kernel/update_radiance.cl", "UpdateRadiance"),
               std::make_pair("./kernel/deposit_sample.cl", "DepositSamples") }
{}

} // CL namespace
} // Rendering namespace
