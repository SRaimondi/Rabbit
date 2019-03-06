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

// Tile rendering data manager struct
struct TileRenderingData
{
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

    TileRenderingData(cl_context context, const TileDescription& tile_description);
};

//// Tile rendering kernels
//struct TileRenderingKernels
//{
//    // Data initialisation kernel
//    cl_kernel initialisation;
//    // Restart sample kernel
//    cl_kernel restart_sample;
//    // Intersect sample's ray with scene
//    cl_kernel intersect_scene;
//    // Update sample radiance kernel
//    cl_kernel update_radiance;
//    // Deposit ray computed radiance to pixels
//    cl_kernel deposit_sample;
//
//    TileRenderingKernels(const cl::Context& context,
//                         const std::pair<const std::string, const std::string>& initialisation_desc,
//                         const std::pair<const std::string, const std::string>& restart_desc,
//                         const std::pair<const std::string, const std::string>& intersect_desc,
//                         const std::pair<const std::string, const std::string>& update_desc,
//                         const std::pair<const std::string, const std::string>& deposit_desc);
//};

class TileRendering
{
public:
    TileRendering(const cl::Context& context, const cl::Device& device,
                  cl_command_queue_properties queue_properties,
                  const SceneDescription& scene_description,
                  const Camera& camera);

    // Access TileDescription from the context
    const TileDescription& GetTileDescription() const noexcept
    {
        return tile_description;
    }

private:
    // Command queue where the commands are issued for the tile
    cl::CommandQueue command_queue;

    // Description of the tile
    const TileDescription tile_description;

    // Rendering data
    TileRenderingData rendering_data;

    // FIXME This are here for the moment
    // Buffer with the spheres in the scene
    const unsigned int num_spheres;
    cl::Buffer d_spheres;

    // Buffer with the camera used to render the scene
    cl::Buffer d_camera;

    // Tile rendering kernels
    TileRenderingKernels kernels;
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_TILERENDERING_HPP
