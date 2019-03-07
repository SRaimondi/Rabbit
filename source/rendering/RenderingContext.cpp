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

RenderingContext::RenderingContext(cl_context context, cl_device_id device,
                                   const SceneDescription& scene_description, const ::CL::Scene& scene)
    : target_context{ context }, target_device{ device },
      tile_rendering_context{ context, device, CL_QUEUE_PROFILING_ENABLE, scene_description, scene }
{
    try
    {
        // Increase reference count on context and device
        CL_CHECK_CALL(clRetainContext(target_context));
        CL_CHECK_CALL(clRetainDevice(target_device));
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

RenderingContext::~RenderingContext() noexcept
{
    Cleanup();
}

void RenderingContext::Cleanup() noexcept
{
    try
    {
        CL_CHECK_CALL(clReleaseDevice(target_device));
        CL_CHECK_CALL(clReleaseContext(target_context));
    }
    catch (const std::exception& ex)
    {
        // TODO operator<< could throw
        std::cerr << ex.what() << std::endl;
    }
}

} // CL namespace
} // Rendering namespace