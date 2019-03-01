//
// Created by Simon on 2019-03-01.
//

#include "FileIO.hpp"

#include <fstream>
#include <sstream>

namespace IO
{

std::string ReadFile(const std::string& filename)
{
    std::ifstream file{ filename };
    if (!file.is_open())
    {
        std::ostringstream error_message;
        error_message << "Could not open file: " << filename;
        throw std::invalid_argument(error_message.str());
    }

    // Read file content
    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

} // IO namespace
