#ifndef SPHERE_H
#define SPHERE_H

// Sphere data layout
typedef struct 
{
    // Origin and radius
    float center_x, center_y, center_z;
    float radius;
} Sphere;

// Intersect ray with sphere
inline bool IntersectRaySphere(const Sphere sphere, 
                               float ray_origin_x, float ray_origin_y, float ray_origin_z,
                               float ray_direction_x, float ray_direction_y, float ray_direction_z,
                               float* ray_extent)
{
    
}

#endif // SPHERE_H
