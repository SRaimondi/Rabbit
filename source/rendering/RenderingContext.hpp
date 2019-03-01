//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_RENDERINGCONTEXT_HPP
#define RABBIT_RENDERINGCONTEXT_HPP

#include "TileRenderingContext.hpp"

namespace Rendering
{
namespace CL
{

// This class is responsible for managing the resources sued during rendering
class RenderingContext
{
public:
    // Create a new rendering context given a device, context properties and the size of the image we are targeting
    RenderingContext(const cl::Device& device, cl_context_properties* contex_prop,
                     const SceneDescription& scene_description,
                     const Camera& camera);

private:
    static void CL_CALLBACK
    ContextCallback(const char* errinfo, const void* private_info, size_t cb, void* user_data);

    // Device where we run our rendering process, this is passed in from outside
    cl::Device target_device;
    // OpenCL context
    cl::Context context;

    // TileRenderingContext is responsible for rendering a certain tile of the image
    TileRenderingContext tile_rendering_context;
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_RENDERINGCONTEXT_HPP
