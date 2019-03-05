#include "kernel_common.h"

// This kernel is called once at the beginning and is reposibile for initalising the data
__kernel void Initialise(__global unsigned int* ray_depth,
                        __global unsigned int* xorshift_state,
                        unsigned int size)
{
    const unsigned int tid = get_global_id(0);
    if (tid < size)
    {
        // Set the ray_depth to RAY_DONE_DEPTH so the first call to the restart kernel sets all rays up
        ray_depth[tid] = RAY_DONE_DEPTH;
        // Seed the xorshift to some value
        xorshift_state[tid] = XORSHIFT_STATE_START + tid;
    }    
}