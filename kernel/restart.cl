#include "kernel_common.h"

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
                            // Sample position in the pixel
                            __global float* sample_x, __global float* sample_y,
                            // XORSHIFT state to generate pixel sample
                            __global unsigned int* xorshift_state
                            )
{
    
}