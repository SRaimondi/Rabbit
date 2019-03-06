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
// Use this for function calls
#define CL_CHECK_CALL(call)     ::CL::CheckCLErrorCode(call, __LINE__, __FILE__)
// Use this to check a status
#define CL_CHECK_STATUS(status) ::CL::CheckCLErrorCode(status, __LINE__, __FILE__)
#else
#define CL_CHECK_CALL(call) static_cast<void>(call)
#define CL_CHECK_STATUS(status)
#endif

// Context callback function
void CL_CALLBACK ContextCallback(const char* errinfo, const void* private_info, size_t cb, void* user_data);

} // CL namespace

#endif //RABBIT_CLERROR_HPP
