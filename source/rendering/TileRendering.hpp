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

// Tile description
struct TileDescription
{
    // Tile size
    const unsigned int tile_width, tile_height;
    // Samples per pixel
    const unsigned int pixel_samples;

    TileDescription(unsigned int tw, unsigned int th, unsigned int ps) noexcept
        : tile_width(tw), tile_height(th), pixel_samples(ps)
    {}

    // Compute the total number of pixels in the tile
    unsigned int TotalPixels() const noexcept
    {
        return tile_width * tile_height;
    }

    // Compute the total number of samples in the tile
    unsigned int TotalSamples() const noexcept
    {
        return TotalPixels() * pixel_samples;
    }
};

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
