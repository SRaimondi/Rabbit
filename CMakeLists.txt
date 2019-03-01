cmake_minimum_required(VERSION 3.13)
project(Rabbit)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

include_directories(source
        source/cl
        source/rendering
        source/utilities)

find_package(OpenCL REQUIRED)

add_executable(Rabbit
        source/main.cpp
        source/cl/CLError.cpp
        source/cl/CLError.hpp
        source/rendering/TracingData.cpp
        source/rendering/TracingData.hpp external/stb_image_writer.hpp source/utilities/FileIO.cpp source/utilities/FileIO.hpp source/cl/CLProgram.cpp source/cl/CLProgram.hpp)

if (APPLE)
    target_compile_definitions(Rabbit PRIVATE CL_SILENCE_DEPRECATION)
endif(APPLE)

target_link_libraries(Rabbit OpenCL::OpenCL)