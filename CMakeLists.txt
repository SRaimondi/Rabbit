cmake_minimum_required(VERSION 3.13)
project(Rabbit)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

include_directories(source
        source/cl
        source/rendering
        source/scene
        source/utilities
        external)

# Try to find OpenCL directly
find_package(OpenCL REQUIRED)


add_executable(Rabbit
        external/stb_image_writer.hpp
        source/utilities/FileIO.cpp
        source/utilities/FileIO.hpp
        source/rendering/RenderingContext.cpp
        source/rendering/RenderingContext.hpp
        source/rendering/RenderingData.cpp
        source/rendering/RenderingData.hpp
        source/utilities/CLError.cpp
        source/utilities/CLError.hpp
        source/utilities/Vector.hpp
        source/utilities/Common.hpp
        source/rendering/Camera.cpp
        source/rendering/Camera.hpp
        source/scene/SceneParser.cpp
        source/scene/SceneParser.hpp
        source/rendering/TileRendering.cpp
        source/rendering/TileRendering.hpp
        source/Main.cpp source/rendering/RenderingKernels.cpp source/rendering/RenderingKernels.hpp)

if (APPLE)
    target_compile_definitions(Rabbit PRIVATE CL_SILENCE_DEPRECATION)
endif(APPLE)

target_link_libraries(Rabbit OpenCL::OpenCL)
