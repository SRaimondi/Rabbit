//
// Created by Simon on 2019-03-01.
//

#include "RenderingData.hpp"
#include "CLError.hpp"

#include <iostream>

namespace Rendering
{
namespace CL
{

// Macro that checks for non-null pointer and releases
#define RELEASE(buffer)                         \
if (buffer != nullptr)                          \
{                                               \
    CL_CHECK_CALL(clReleaseMemObject(buffer));  \
}

Rays::Rays(cl_context context, unsigned int num_rays)
    : num_rays{ num_rays },
      origin_x{ nullptr }, origin_y{ nullptr }, origin_z{ nullptr },
      direction_x{ nullptr }, direction_y{ nullptr }, direction_z{ nullptr },
      depth{ nullptr }
{
    cl_int err_code{ CL_SUCCESS };
    const size_t buffer_size{ num_rays * sizeof(float) };

    try
    {
        origin_x = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        origin_y = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        origin_z = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);

        direction_x = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        direction_y = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        direction_z = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);

        depth = clCreateBuffer(context, CL_MEM_READ_WRITE, num_rays * sizeof(cl_uint), nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

Rays::~Rays() noexcept
{
    Cleanup();
}

void Rays::Cleanup() noexcept
{
    try
    {
        RELEASE(origin_x)
        RELEASE(origin_y)
        RELEASE(origin_z)
        RELEASE(direction_x)
        RELEASE(direction_y)
        RELEASE(direction_z)
        RELEASE(depth)
    }
    catch (const std::exception& ex)
    {
        // FIXME operator<< could throw
        std::cerr << ex.what() << std::endl;
    }
}

Intersections::Intersections(cl_context context, unsigned int num_intersections)
    : num_intersections(num_intersections),
      hit_point_x{ nullptr }, hit_point_y{ nullptr }, hit_point_z{ nullptr },
      normal_x{ nullptr }, normal_y{ nullptr }, normal_z{ nullptr },
      uv_s{ nullptr }, uv_t{ nullptr },
      primitive_index{ nullptr }
{
    cl_int err_code{ CL_SUCCESS };
    const size_t buffer_size{ num_intersections * sizeof(float) };

    try
    {
        hit_point_x = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        hit_point_y = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        hit_point_z = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);

        normal_x = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        normal_y = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        normal_z = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);

        uv_s = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        uv_t = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);

        primitive_index = clCreateBuffer(context, CL_MEM_READ_WRITE, num_intersections * sizeof(cl_uint), nullptr,
                                         &err_code);
        CL_CHECK_STATUS(err_code);
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

Intersections::~Intersections() noexcept
{
    Cleanup();
}

void Intersections::Cleanup() noexcept
{
    try
    {
        RELEASE(hit_point_x)
        RELEASE(hit_point_y)
        RELEASE(hit_point_z)
        RELEASE(normal_x)
        RELEASE(normal_y)
        RELEASE(normal_z)
        RELEASE(uv_s)
        RELEASE(uv_t)
        RELEASE(primitive_index)
    }
    catch (const std::exception& ex)
    {
        // TODO operator<< could throw
        std::cerr << ex.what() << std::endl;
    }
}

Samples::Samples(cl_context context, unsigned int num_samples)
    : num_samples{ num_samples },
      Li_r{ nullptr }, Li_g{ nullptr }, Li_b{ nullptr },
      beta_r{ nullptr }, beta_g{ nullptr }, beta_b{ nullptr },
      pixel_x{ nullptr }, pixel_y{ nullptr },
      sample_offset_x{ nullptr }, sample_offset_y{ nullptr }
{
    cl_int err_code{ CL_SUCCESS };
    const size_t buffer_size{ num_samples * sizeof(float) };

    try
    {
        Li_r = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        Li_g = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        Li_b = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);

        beta_r = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        beta_g = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        beta_b = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);

        pixel_x = clCreateBuffer(context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_uint), nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        pixel_y = clCreateBuffer(context, CL_MEM_READ_WRITE, num_samples * sizeof(cl_uint), nullptr, &err_code);
        CL_CHECK_STATUS(err_code);

        sample_offset_x = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        sample_offset_y = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

Samples::~Samples() noexcept
{
    Cleanup();
}

void Samples::Cleanup() noexcept
{
    try
    {
        RELEASE(Li_r)
        RELEASE(Li_g)
        RELEASE(Li_b)
        RELEASE(beta_r)
        RELEASE(beta_g)
        RELEASE(beta_b)
        RELEASE(pixel_x)
        RELEASE(pixel_y)
        RELEASE(sample_offset_x)
        RELEASE(sample_offset_y)
    }
    catch (const std::exception& ex)
    {
        // TODO operator<< could throw
        std::cerr << ex.what() << std::endl;
    }
}

Pixels::Pixels(cl_context context, unsigned int num_pixels)
    : num_pixels(num_pixels),
      pixel_r{ nullptr }, pixel_g{ nullptr }, pixel_b{ nullptr },
      filter_weight{ nullptr }
{
    cl_int err_code{ CL_SUCCESS };
    const size_t buffer_size{ num_pixels * sizeof(float) };

    try
    {
        pixel_r = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        pixel_g = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
        pixel_b = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);

        filter_weight = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

Pixels::~Pixels() noexcept
{
    Cleanup();
}

void Pixels::Cleanup() noexcept
{
    try
    {
        RELEASE(pixel_r)
        RELEASE(pixel_g)
        RELEASE(pixel_b)
        RELEASE(filter_weight)
    }
    catch (const std::exception& ex)
    {
        // TODO operator<< could throw
        std::cerr << ex.what() << std::endl;
    }
}

XOrShift::XOrShift(cl_context context, unsigned int num_generators)
    : num_generators{ num_generators },
      state{ nullptr }
{
    cl_int err_code{ CL_SUCCESS };

    try
    {
        state = clCreateBuffer(context, CL_MEM_READ_WRITE, num_generators * sizeof(cl_uint), nullptr, &err_code);
        CL_CHECK_STATUS(err_code);
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

XOrShift::~XOrShift() noexcept
{
    Cleanup();
}

void XOrShift::Cleanup() noexcept
{
    try
    {
        RELEASE(state)
    }
    catch (const std::exception& ex)
    {
        // TODO operator<< could throw
        std::cerr << ex.what() << std::endl;
    }
}

} // CL namespace
} // Rendering namespace
