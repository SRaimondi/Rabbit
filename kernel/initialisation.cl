#include "kernel_common.h"

// This kernel is called once at the beginning and is reposibile for initalising the data
__kernel void Initialise(
                         // Depth of the ray
                         __global unsigned int* ray_depth,
                         // Random number generator state
                         __global unsigned int* xorshift_state,
                         // Accumulted value for each pixel and filter weight
                         __global float* pixel_r, __global float* pixel_g, __global float* pixel_b,
                         __global float* filter_weight,
                         // Pixel coordinates
                         __global unsigned int* pixel_x, __global unsigned int* pixel_y,
                         // Description of the tile
                         unsigned int tile_start_x, unsigned int tile_start_y,
                         unsigned int tile_width, unsigned int tile_height,
                         unsigned int samples_per_pixel)
{
    const unsigned int tid = get_global_id(0);
    if (tid < tile_width * tile_height * samples_per_pixel)
    {
        // Set the ray_depth to RAY_DONE_DEPTH so the first call to the restart kernel sets all rays up
        ray_depth[tid] = RAY_DONE_DEPTH;
        
        // Seed the xorshift to some value
        xorshift_state[tid] = XORSHIFT_STATE_START + tid;

        // Set the pixel value and filter weight to 0
        pixel_r[tid] = 0.f;
        pixel_g[tid] = 0.f;
        pixel_b[tid] = 0.f;
        filter_weight[tid] = 0.f;

        // Compute coordiantes of the pixel
        // Samples are stored along width and packed for each pixel
        const unsigned int linear_pixel_index = tid / samples_per_pixel;
        const unsigned int py = linear_pixel_index / tile_width;
        pixel_x[tid] = linear_pixel_index - py * tile_width;
        pixel_y[linear_index] = py;
    }    
}