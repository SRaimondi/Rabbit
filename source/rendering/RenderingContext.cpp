//
// Created by Simon on 2019-03-01.
//

#include "RenderingContext.hpp"
#include "CLError.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace CL
{
    RenderingContext::RenderingContext(const cl::Device& device, cl_context_properties* contex_prop,
                                       unsigned int image_width, unsigned int image_height)
    try
        : target_device(device), context(device, contex_prop, RenderingContext::ContextCallback),
          d_rays(context, image_width * image_height), d_intersections(context, image_width * image_height),
          d_samples(context, image_width * image_height), d_xorshift_state(context, image_width * image_height)
    {}
    catch (const cl::Error& err)
    {
        std::ostringstream error_message;
        error_message << "Error during RenderingContext creation\n" << err.what() << " "
                      << ErrorCodeToString(err.err());
        throw std::runtime_error(error_message.str());
    }

    void RenderingContext::ContextCallback(const char* errinfo, const void*, size_t, void*)
    {
        std::cerr << "OpenCL context callback: " << errinfo << std::endl;
    }

} // CL namespace