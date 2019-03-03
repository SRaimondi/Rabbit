//
// Created by Simon on 2019-03-01.
//

#include "TileRendering.hpp"

namespace Rendering
{
namespace CL
{

TileRendering::TileRendering(const cl::Context& context, const cl::Device& device,
                             cl_command_queue_properties queue_properties,
                             const SceneDescription& scene_description,
                             const Camera& camera)
    : command_queue{ context, device, queue_properties },
      tile_description{ scene_description.tile_width, scene_description.tile_height, scene_description.pixel_samples },
      d_rays{ context, tile_description.TotalSamples() },
      d_intersections{ context, tile_description.TotalSamples() },
      d_samples{ context, tile_description.TotalSamples() },
      d_pixels{ context, tile_description.TotalSamples() },
      d_xorshift_state{ context, tile_description.TotalSamples() },
      num_spheres{ static_cast<unsigned int>(scene_description.loaded_spheres.size()) },
      d_spheres{ context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, num_spheres * sizeof(Sphere),
                 const_cast<Sphere*>(scene_description.loaded_spheres.data()) },
      d_camera{ context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Camera), const_cast<Camera*>(&camera) }
{}

} // CL namespace
} // Rendering namespace
