//
// Created by Simon on 2019-03-01.
//

#include "RenderingContext.hpp"
#include "CLError.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Rendering
{
namespace CL
{

RenderingContext::RenderingContext(const cl::Device& device, cl_context_properties* contex_prop,
                                   const SceneDescription& scene_description,
                                   const Camera& camera)
    : target_device{ device }, context{ device, contex_prop, RenderingContext::ContextCallback },
      tile_rendering_context{ context, target_device, CL_QUEUE_PROFILING_ENABLE, scene_description, camera }
{}

void RenderingContext::ContextCallback(const char* errinfo, const void*, size_t, void*)
{
    std::cerr << "OpenCL context callback: " << errinfo << std::endl;
}

} // CL namespace
} // Rendering namespace