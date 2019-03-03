//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_TILERENDERING_HPP
#define RABBIT_TILERENDERING_HPP

#include "RenderingData.hpp"
#include "SceneParser.hpp"
#include "Camera.hpp"

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

    // Compute the total number of samples in the tile
    unsigned int TotalSamples() const noexcept
    {
        return tile_width * tile_height * pixel_samples;
    }
};

class TileRendering
{
public:
    TileRendering(const cl::Context& context, const cl::Device& device,
                  cl_command_queue_properties queue_properties,
                  const SceneDescription& scene_description,
                  const Camera& camera);

    // Access TileDescription from the context
    const TileDescription& TileDesc() const noexcept
    {
        return tile_description;
    }

private:
    // Command queue where the commands are issued for the tile
    cl::CommandQueue command_queue;

    // Description of the tile
    const TileDescription tile_description;

    // Device rays, one for each sample for each pixel
    Rays d_rays;
    // Intersection information
    Intersections d_intersections;
    // Information on the incoming radiance for each sample
    Samples d_samples;
    // Accumulated value and filter weight for each pixel in the tile
    Pixels d_pixels;
    // XOrShift state for random number generation
    XOrShift d_xorshift_state;

    // Buffer with the spheres in the scene
    const unsigned int num_spheres;
    cl::Buffer d_spheres;

    // Buffer with the camera used to render the scene
    cl::Buffer d_camera;
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_TILERENDERING_HPP
