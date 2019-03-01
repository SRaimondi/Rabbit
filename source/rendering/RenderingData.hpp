//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_RENDERINGDATA_HPP
#define RABBIT_RENDERINGDATA_HPP

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#include "cl2.hpp"

namespace CL
{
    // Storage class for the Rays data
    struct Rays
    {
        Rays(const cl::Context& context, unsigned int num_rays);

        const unsigned int num_rays;

        // Origin
        cl::Buffer origin_x;
        cl::Buffer origin_y;
        cl::Buffer origin_z;

        // Direction
        cl::Buffer direction_x;
        cl::Buffer direction_y;
        cl::Buffer direction_z;

        // Ray extent
        cl::Buffer extent;

        // Current depth of the ray (cl_uint)
        cl::Buffer depth;
    };

    // Storage class for the intersection information
    struct Intersections
    {
        Intersections(const cl::Context& context, unsigned int num_intersections);

        const unsigned int num_intersections;

        // Hit point
        cl::Buffer hit_point_x;
        cl::Buffer hit_point_y;
        cl::Buffer hit_point_z;

        // Normal
        cl::Buffer normal_x;
        cl::Buffer normal_y;
        cl::Buffer normal_z;

        // UV coordinates
        cl::Buffer uv_s;
        cl::Buffer uv_t;

        // Index of the intersected primitive (cl_uint)
        cl::Buffer primitive_index;
    };

    // Samples taken in the image plane
    struct Samples
    {
        Samples(const cl::Context& context, unsigned int num_samples);

        const unsigned int num_samples;

        // Incoming radiance
        cl::Buffer Li_r;
        cl::Buffer Li_g;
        cl::Buffer Li_b;

        // Accumulation mask for current path
        cl::Buffer beta_r;
        cl::Buffer beta_g;
        cl::Buffer beta_b;

        // Sample pixel coordinates
        cl::Buffer pixel_x;
        cl::Buffer pixel_y;

        // Sample offset in the pixel
        cl::Buffer sample_offset_x;
        cl::Buffer sample_offset_y;
    };

    // Film pixels
    struct Pixels
    {
        Pixels(const cl::Context& context, unsigned int num_pixels);

        const unsigned int num_pixels;

        // Accumulated pixel value
        cl::Buffer pixel_r;
        cl::Buffer pixel_g;
        cl::Buffer pixel_b;

        // Total filter value
        cl::Buffer filter_weight;
    };

    // XOrShift status, it's a very simple generator but good enough for testing and also has 32bit status
    struct XOrShift
    {
        XOrShift(const cl::Context& context, unsigned int num_generators);

        const unsigned int num_generators;

        // Status of the generator
        cl::Buffer state;
    };
}

#endif //RABBIT_RENDERINGDATA_HPP
