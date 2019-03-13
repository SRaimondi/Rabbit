//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_RENDERINGDATA_HPP
#define RABBIT_RENDERINGDATA_HPP

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

namespace Rendering
{
namespace CL
{

// Storage class for the Rays data
class Rays
{
public:
    Rays(cl_context context, unsigned int num_rays);

    ~Rays() noexcept;

    const unsigned int num_rays;

    // Origin
    cl_mem origin_x;
    cl_mem origin_y;
    cl_mem origin_z;

    // Direction
    cl_mem direction_x;
    cl_mem direction_y;
    cl_mem direction_z;

    // Current depth of the ray (cl_uint)
    cl_mem depth;

private:
    // Cleanup all buffers without throwing
    void Cleanup() noexcept;
};

// Storage class for the intersection information
class Intersections
{
public:
    Intersections(cl_context context, unsigned int num_intersections);

    ~Intersections() noexcept;

    const unsigned int num_intersections;

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

    // Outgoing light direction at intersection
    cl_mem wo_x;
    cl_mem wo_y;
    cl_mem wo_z;

    // Index of the intersected primitive (cl_uint)
    cl_mem primitive_index;

private:
    // Cleanup all buffers without throwing
    void Cleanup() noexcept;
};

// Samples taken in the image plane
class Samples
{
public:
    Samples(cl_context context, unsigned int num_samples);

    ~Samples() noexcept;

    const unsigned int num_samples;

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

    // Number of samples done, single cl_uint
    cl_mem samples_done;

private:
    // Cleanup all buffers without throwing
    void Cleanup() noexcept;
};

// Film pixels
class Pixels
{
public:
    Pixels(cl_context context, unsigned int num_pixels);

    ~Pixels() noexcept;

    const unsigned int num_pixels;

    // Accumulated pixel value
    cl_mem pixel_r;
    cl_mem pixel_g;
    cl_mem pixel_b;

    // Total filter value
    cl_mem filter_weight;

private:
    // Cleanup all buffers without throwing
    void Cleanup() noexcept;
};

// XOrShift status, it's a very simple generator but good enough for testing and also has 32bit status
class XOrShift
{
public:
    XOrShift(cl_context context, unsigned int num_generators);

    ~XOrShift() noexcept;

    const unsigned int num_generators;

    // Status of the generator
    cl_mem state;

private:
    // Cleanup all buffers without throwing
    void Cleanup() noexcept;
};

// Rendering data storage
struct RenderingData
{
    // Device rays, one for each sample for each pixel
    Rays d_rays;
    // Intersection information
    Intersections d_intersections;
    // Information on the incoming radiance for each sample
    Samples d_samples;
    // Accumulated value and filter weight for each pixel in the tile
    Pixels d_pixels;
    // XOrShift state for random number generation
    XOrShift d_xorshift_state;

    RenderingData(cl_context context, unsigned int num_pixels, unsigned int total_tile_samples);
};

} // CL namespace
} // Rendering namespace

#endif //RABBIT_RENDERINGDATA_HPP
