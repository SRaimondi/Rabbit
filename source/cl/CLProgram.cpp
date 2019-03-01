//
// Created by Simon on 2019-03-01.
//

#include "CLProgram.hpp"

#include <memory>
#include <sstream>

namespace CL
{
    CLPRogram::CLPRogram(cl_context context, const std::string& source)
        : is_compiled{ false }, is_linked{ false }
    {
        cl_int err_code{ CL_SUCCESS };
        const char* str = source.c_str();
        program = clCreateProgramWithSource(context, 1, &str, nullptr, &err_code);
        CHECK_CL(err_code);
    }

    CLPRogram::CLPRogram(cl_context context, const std::vector<std::string>& sources)
        : is_compiled{ false }, is_linked{ false }
    {
        // Put string into vector
        std::vector<const char*> strs{ sources.size() };
        for (size_t i = 0; i != sources.size(); i++)
        {
            strs[i] = sources[i].c_str();
        }

        // Create program
        cl_int err_code{ CL_SUCCESS };
        program = clCreateProgramWithSource(context, static_cast<cl_uint>(strs.size()), &strs[0], nullptr, &err_code);
        CHECK_CL(err_code);
    }

    CLPRogram::~CLPRogram() noexcept
    {
        CHECK_LOG_CL(clReleaseProgram(program));
    }

    void CLPRogram::Build(cl_device_id device, const std::string& options)
    {
        cl_int err_code = clBuildProgram(program,
                                         1, &device,
                                         options.c_str(), nullptr, nullptr);
        if (err_code != CL_SUCCESS)
        {
            const auto build_log = ProgramBuildLog(device);
            if (!build_log.empty())
            {
                std::ostringstream error_message;
                error_message << "Program build log\n" << build_log;
                throw std::runtime_error(error_message.str());
            }
        }
        else
        {
            is_compiled = true;
            is_linked = true;
        }
    }

    std::string CLPRogram::ProgramBuildLog(cl_device_id device) const
    {
        size_t log_size{ 0 };
        CHECK_CL(clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size));
        if (log_size != 0)
        {
            std::unique_ptr<char[]> log = std::make_unique<char[]>(log_size);
            CHECK_CL(clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.get(), nullptr));
            return std::string{ log.get() };
        }

        return std::string{};
    }

} // CL namespace
