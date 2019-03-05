#include "kernel_common.h"
#include "camera.h"

// This kernel is responsible for restarting all the samples that are done
__kernel void RestartSample(__constant const Camera* camera,
                            // Ray data to reset
                            __global float* ray_origin_x, __global float* ray_origin_y, __global float* ray_origin_z,
                            __global float* ray_direction_x, __global float* ray_direction_y, __global float* ray_direction_z,
                            __global float* ray_extent, __global unsigned int* ray_depth,
                            // Intersection data to reset
                            __global unsigned int* primitive_index,
                            // Samples data to reset
                            __global float* Li_r, __global float* Li_g, __global float* Li_b,
                            __global float* beta_r, __global float* beta_g, __global float* beta_b,
                            // Pixel information
                            __global const unsigned int* pixel_x, __global const unsigned int* pixel_y,
                            __global float* sample_x, __global float* sample_y,
                            // XORSHIFT state to generate pixel sample
                            __global unsigned int* xorshift_state
                            // Size of the tile
                            unsigned int tile_width, unsigned int tile_height)
{
    const unsigned int tid_x = get_global_id(0);
    const unsigned int tid_y = get_global_id(1);

    // Check we are in the tile domain
    if (tid_x < tile_width && tid_y < tile_height)
    {
        const unsigned int linear_index = tid_y * tile_width + tid_x;
        // Check if the ray is done
        if (ray_depth[linear_index] == RAY_DONE_DEPTH)
        {
            // Sample new offset in the pixel
            const float sx = GenerateFloat(xorshift_state + linear_index);
            const float sy = GenerateFloat(xorshift_state + linear_index);

            // Generate ray for the sample
            float3 eye, dir;
            GenerateRay(camera, pixel_x[linear_index], pixel_y[linear_index], sx, sy, &eye, &dir);


            // Reset primitive index
            primitive_index[linear_index] = INVALID_PRIM_INDEX;

            
        }
    }
}