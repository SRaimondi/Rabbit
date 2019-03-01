//
// Created by Simon on 2019-03-01.
//

#include "TracingData.hpp"

Rays::Rays(cl_context context, unsigned int num_rays)
    : num_rays{ num_rays }
{
    cl_int err_code{ CL_SUCCESS };
    const size_t alloc_size_f = Size() * sizeof(cl_float);

    // Allocate memory on the device
    origin_x = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    origin_y = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    origin_z = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);

    direction_x = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    direction_y = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    direction_z = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);

    extent = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);

    depth = clCreateBuffer(context, CL_MEM_READ_WRITE, Size() * sizeof(cl_uint), nullptr, &err_code);
    CHECK_CL(err_code);
}

Rays::~Rays() noexcept
{
    CHECK_LOG_CL(clReleaseMemObject(origin_x));
    CHECK_LOG_CL(clReleaseMemObject(origin_y));
    CHECK_LOG_CL(clReleaseMemObject(origin_z));

    CHECK_LOG_CL(clReleaseMemObject(direction_x));
    CHECK_LOG_CL(clReleaseMemObject(direction_y));
    CHECK_LOG_CL(clReleaseMemObject(direction_z));

    CHECK_LOG_CL(clReleaseMemObject(extent));
    CHECK_LOG_CL(clReleaseMemObject(depth));
}

Intersections::Intersections(cl_context context, unsigned int num_intersections)
    : num_intersections(num_intersections)
{
    cl_int err_code{ CL_SUCCESS };
    const size_t alloc_size_f = Size() * sizeof(cl_float);

    // Allocate memory on the device
    hit_point_x = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    hit_point_y = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    hit_point_z = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);

    normal_x = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    normal_y = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    normal_z = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);

    uv_s = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    uv_t = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);

    primitive_index = clCreateBuffer(context, CL_MEM_READ_WRITE, Size() * sizeof(cl_uint), nullptr, &err_code);
    CHECK_CL(err_code);
}

Intersections::~Intersections() noexcept
{
    CHECK_LOG_CL(clReleaseMemObject(hit_point_x));
    CHECK_LOG_CL(clReleaseMemObject(hit_point_y));
    CHECK_LOG_CL(clReleaseMemObject(hit_point_z));

    CHECK_LOG_CL(clReleaseMemObject(normal_x));
    CHECK_LOG_CL(clReleaseMemObject(normal_y));
    CHECK_LOG_CL(clReleaseMemObject(normal_z));

    CHECK_LOG_CL(clReleaseMemObject(uv_s));
    CHECK_LOG_CL(clReleaseMemObject(uv_t));

    CHECK_LOG_CL(clReleaseMemObject(primitive_index));
}

Samples::Samples(cl_context context, unsigned int num_samples)
    : num_samples{ num_samples }
{
    cl_int err_code{ CL_SUCCESS };
    const size_t alloc_size_f = Size() * sizeof(cl_float);

    // Allocate memory on the device
    Li_r = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    Li_g = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    Li_b = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);

    beta_r = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    beta_g = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    beta_b = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);

    pixel_x = clCreateBuffer(context, CL_MEM_READ_WRITE, Size() * sizeof(cl_uint), nullptr, &err_code);
    CHECK_CL(err_code);
    pixel_y = clCreateBuffer(context, CL_MEM_READ_WRITE, Size() * sizeof(cl_uint), nullptr, &err_code);
    CHECK_CL(err_code);

    sample_offset_x = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
    sample_offset_y = clCreateBuffer(context, CL_MEM_READ_WRITE, alloc_size_f, nullptr, &err_code);
    CHECK_CL(err_code);
}

Samples::~Samples() noexcept
{
    CHECK_LOG_CL(clReleaseMemObject(Li_r));
    CHECK_LOG_CL(clReleaseMemObject(Li_g));
    CHECK_LOG_CL(clReleaseMemObject(Li_b));

    CHECK_LOG_CL(clReleaseMemObject(beta_r));
    CHECK_LOG_CL(clReleaseMemObject(beta_g));
    CHECK_LOG_CL(clReleaseMemObject(beta_b));

    CHECK_LOG_CL(clReleaseMemObject(pixel_x));
    CHECK_LOG_CL(clReleaseMemObject(pixel_y));

    CHECK_LOG_CL(clReleaseMemObject(sample_offset_x));
    CHECK_LOG_CL(clReleaseMemObject(sample_offset_y));
}

XOrShift::XOrShift(cl_context context, unsigned int num_generators)
    : num_generators{ num_generators }
{
    cl_int err_code = CL_SUCCESS;
    state = clCreateBuffer(context, CL_MEM_READ_WRITE, Size() * sizeof(cl_uint), nullptr, &err_code);
    CHECK_CL(err_code);
}

XOrShift::~XOrShift() noexcept
{
    CHECK_LOG_CL(clReleaseMemObject(state));
}

