//
// Created by Simon on 2019-03-01.
//

#include "RenderingContext.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Rendering
{
namespace CL
{

RenderingContext::RenderingContext(cl_device_id device, cl_context_properties* contex_prop,
                                   const SceneDescription& scene_description,
                                   const Camera& camera)
    : target_device{ device }, context{ nullptr },
      tile_rendering_context{ context, target_device, CL_QUEUE_PROFILING_ENABLE, scene_description, camera }
{}

void RenderingContext::ContextCallback(const char* errinfo, const void*, size_t, void*)
{
    std::cerr << "OpenCL context callback: " << errinfo << std::endl;
}

} // CL namespace
} // Rendering namespace