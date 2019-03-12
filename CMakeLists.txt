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
find_package(OpenCL)

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
        source/Main.cpp
        source/rendering/RenderingKernels.cpp
        source/rendering/RenderingKernels.hpp
        source/scene/Scene.cpp
        source/scene/Scene.hpp
        source/rendering/TileDescription.hpp)

if (APPLE)
    target_compile_definitions(Rabbit PRIVATE CL_SILENCE_DEPRECATION)
endif(APPLE)

if (NOT OpenCL_FOUND)
    # TODO This is hardocoded to the CUDA folder
    target_include_directories(Rabbit PRIVATE $ENV{CUDA_PATH}/include)
    target_link_libraries(Rabbit PRIVATE $ENV{CUDA_PATH}/lib/x64/OpenCL.lib)
else()
    target_link_libraries(Rabbit OpenCL::OpenCL)
endif()

# Specify flags for build
IF (CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_COMPILE_OPTIONS(Rabbit PRIVATE -Wall -Wextra)
ENDIF ()

