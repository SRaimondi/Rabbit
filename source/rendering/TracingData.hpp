//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_TRACINGDATA_HPP
#define RABBIT_TRACINGDATA_HPP

#include "CLError.hpp"

// Storage class for the Rays data
struct Rays
{
    Rays(cl_context context, unsigned int num_rays);

    ~Rays() noexcept;

    // Origin
    cl_mem origin_x;
    cl_mem origin_y;
    cl_mem origin_z;

    // Direction
    cl_mem direction_x;
    cl_mem direction_y;
    cl_mem direction_z;

    // Ray extent
    cl_mem extent;

    // Current depth of the ray (cl_uint)
    cl_mem depth;

    unsigned int Size() const noexcept
    {
        return num_rays;
    }

private:
    const unsigned int num_rays;
};

// Storage class for the intersection information
struct Intersections
{
    explicit Intersections(cl_context context, unsigned int num_intersections);

    ~Intersections() noexcept;

    // Hit point
    cl_mem hit_point_x;
    cl_mem hit_point_y;
    cl_mem hit_point_z;

    // Normal
    cl_mem normal_x;
    cl_mem normal_y;
    cl_mem normal_z;

    // UV coordinates
    cl_mem uv_s;
    cl_mem uv_t;

    // Index of the intersected primitive
    cl_mem primitive_index;

    unsigned int Size() const noexcept
    {
        return num_intersections;
    }

private:
    const unsigned int num_intersections;
};

// Samples taken in the image plane
struct Samples
{
    Samples(cl_context context, unsigned int num_samples);

    ~Samples() noexcept;

    // Incoming radiance
    cl_mem Li_r;
    cl_mem Li_g;
    cl_mem Li_b;

    // Accumulation mask for current path
    cl_mem beta_r;
    cl_mem beta_g;
    cl_mem beta_b;

    // Sample pixel coordinates
    cl_mem pixel_x;
    cl_mem pixel_y;

    // Sample offset in the pixel
    cl_mem sample_offset_x;
    cl_mem sample_offset_y;

    unsigned int Size() const noexcept
    {
        return num_samples;
    }

private:
    const unsigned int num_samples;
};

// XOrShift status, it's a very simple generator but good enough for testing and also has 32bit status
struct XOrShift
{
    XOrShift(cl_context context, unsigned int num_generators);

    ~XOrShift() noexcept;

    // Status of the generator
    cl_mem state;

    unsigned int Size() const noexcept
    {
        return num_generators;
    }

private:
    const unsigned int num_generators;
};

#endif //RABBIT_TRACINGDATA_HPP
