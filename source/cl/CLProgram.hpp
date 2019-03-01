//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_CLPROGRAM_HPP
#define RABBIT_CLPROGRAM_HPP

#include "CLError.hpp"

#include <vector>

namespace CL
{
    // Define a wrapper class to the cl_program type
    class CLPRogram
    {
    public:
        // Create program from a single source
        CLPRogram(cl_context context, const std::string& source);

        // Create program from multiple sources
        CLPRogram(cl_context context, const std::vector<std::string>& sources);

        ~CLPRogram() noexcept;

        // Build program
        void Build(cl_device_id devices, const std::string& options);

    private:
        // Get program build log
        std::string ProgramBuildLog(cl_device_id device) const;

        // Program status flags
        bool is_compiled, is_linked;

        // Actual program
        cl_program program;
    };

} // CL namespace

#endif //RABBIT_CLPROGRAM_HPP
