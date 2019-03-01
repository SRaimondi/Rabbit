//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_CLERROR_HPP
#define RABBIT_CLERROR_HPP

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <string>

namespace CL
{
    // Convert error code to string
    std::string ErrorCodeToString(cl_int err_code) noexcept;
}

#endif //RABBIT_CLERROR_HPP
