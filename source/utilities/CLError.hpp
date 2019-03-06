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

// Check error status and throw exception
void CheckCLErrorCode(cl_int err_code, int line, const char* file);

// Macro to check and OpenCL error
#ifndef NDEBUG
#define CL_CHECK_CALL(call)     CL::CheckCLErrorCode(call, __LINE__, __FILE__)
#define CL_CHECK_STATUS(status) CL::CheckCLErrorCode(call, __LINE__, __FILE__)
#else
#define CL_CHECK_CALL(call) call
#define CL_CHECK_STATUS(status)
#endif

}

#endif //RABBIT_CLERROR_HPP
