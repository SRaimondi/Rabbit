//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_RENDERINGCONTEXT_HPP
#define RABBIT_RENDERINGCONTEXT_HPP

#include "RenderingData.hpp"
#include "SceneParser.hpp"

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
                     const SceneDescription& scene_description);

private:
    static void CL_CALLBACK
    ContextCallback(const char* errinfo, const void* private_info, size_t cb, void* user_data);

    // Device where we run our rendering process, this is passed in from outside
    cl::Device target_device;
    // OpenCL context
    cl::Context context;

    // Target image width and height
    const unsigned int target_image_width, target_image_height;

    // Programs and kernels

    // Data for the kernels
    Rays d_rays;
    Intersections d_intersections;
    Samples d_samples;
    Pixels d_pixels;
    XOrShift d_xorshift_state;
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_RENDERINGCONTEXT_HPP
