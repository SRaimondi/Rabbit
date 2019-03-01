cmake_minimum_required(VERSION 3.13)
project(Rabbit)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

include_directories(source source/cl)

find_package(OpenCL REQUIRED)

add_executable(Rabbit
        source/main.cpp
        source/cl/CLError.cpp
        source/cl/CLError.hpp)

target_link_libraries(Rabbit OpenCL::OpenCL)