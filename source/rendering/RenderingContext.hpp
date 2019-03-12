//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_RENDERINGCONTEXT_HPP
#define RABBIT_RENDERINGCONTEXT_HPP

#include "TileRendering.hpp"

namespace Rendering
{
namespace CL
{

// This class is responsible for managing the resources used during rendering
class RenderingContext
{
public:
    // Create a new rendering context with a single device, the scene description and a camera to use
    RenderingContext(cl_context context, cl_device_id device,
                     const SceneDescription& scene_description,
                     const ::CL::Scene& scene);

    ~RenderingContext() noexcept;

    // Render image
    void Render() const;

private:
    // Cleanup OpenCL resource without throwing
    void Cleanup() noexcept;

    // OpenCL context and associated device
    cl_context target_context;
    cl_device_id target_device;

    // TileRendering is responsible for rendering a certain tile of the image
    TileRendering tile_rendering_context;
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_RENDERINGCONTEXT_HPP
