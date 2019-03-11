//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_TILERENDERING_HPP
#define RABBIT_TILERENDERING_HPP

#include "RenderingKernels.hpp"

namespace Rendering
{
namespace CL
{

class TileRendering
{
public:
    TileRendering(cl_context context, cl_device_id device, cl_command_queue_properties queue_properties,
                  const SceneDescription& scene_description, const ::CL::Scene& scene);

    ~TileRendering() noexcept;

    // Access TileDescription from the context
    const TileDescription& GetTileDescription() const noexcept
    {
        return tile_description;
    }

private:
    // Cleanup OpenCL resource without throwing
    void Cleanup() noexcept;

    // Command queue where the commands are issued for the tile rendering
    cl_command_queue command_queue;

    // Description of the tile
    const TileDescription tile_description;

    // Rendering data
    RenderingData rendering_data;

    // Rendering kernels
    RenderingKernels rendering_kernel;
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_TILERENDERING_HPP
