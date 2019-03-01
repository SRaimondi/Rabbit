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
    // Convert OpenCL error code to string
    std::string ErrorToSTring(cl_int err_code) noexcept;

    // Check status and throw a runtime exception if an error is found
    void CheckCLErrorHelper(cl_int err_code, int line, const char* file);

} // CL namespace

// Error check macro with argument replacement
#define CHECK_CL(err_code) CL::CheckCLErrorHelper(err_code, __LINE__, __FILE__)


#endif //RABBIT_CLERROR_HPP
