#include "sphere.h"

// Intersect rays with a list of sphere and fill the intersection data
__kernel void Intersect(// Target spheres
                        __global const Sphere* spheres, unsigned int num_spheres,
                        // Ray data
                        __global const float* ray_origin_x, __global const float* ray_origin_y, __global const float* ray_origin_z,
                        __global const float* ray_direction_x, __global const float* ray_direction_y, __global const float* ray_direction_z,
                        __global const float* ray_extent,
                        // Intersection informations
                        __global float* hit_point_x, __global float* hit_point_y, __global float* hit_point_z,
                        __global float* normal_x, __global float* normal_y, __global float* normal_z,
                        __global float* uv_s, __global float* uv_t,
                        __global unsigned int* primitive_index,
                        // Total number of rays
                        unsigned int num_rays)
{
    const unsigned int tid = get_global_id(0);
    if (tid < num_rays)
    {
        // Load ray data
        const float ro_x = ray_origin_x[tid];
        const float ro_y = ray_origin_y[tid];
        const float ro_z = ray_origin_z[tid];
        const float rd_x = ray_direction_x[tid];
        const float rd_y = ray_direction_y[tid];
        const float rd_z = ray_direction_z[tid];
        float r_extent = ray_extent[tid];

        // Check if we intersect any sphere
        unsigned int closest_sphere_index = num_spheres;
        for (unsigned int s = 0; s != num_spheres; s++)
        {
            if (IntersectRaySphere(spheres[s], ro_x, ro_y, ro_z, rd_x, rd_y, rd_z, &r_extent))
            {
                closest_sphere_index = s;
            }
        }

        // If we have a hit, fill the intersection
        if (closest_sphere_index < num_spheres) 
        {
            // Compute hit point
            hit_point_x[tid] = ro_x + r_extent * rd_x;
            hit_point_y[tid] = ro_y + r_extent * rd_y;
            hit_point_z[tid] = ro_z + r_extent * rd_z;

             // Load closest sphere
            const Sphere sphere = spheres[closest_sphere_index];

            // Compute normal
            const float inv_radius = 1.f / sphere.radius;
            normal_x[tid] = (hit_point_x[tid] - sphere.center_x) * inv_radius;
            normal_y[tid] = (hit_point_y[tid] - sphere.center_y) * inv_radius;
            normal_z[tid] = (hit_point_z[tid] - sphere.center_z) * inv_radius;

        }
    }
}