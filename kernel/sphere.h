#ifndef SPHERE_H
#define SPHERE_H

// Sphere data layout
typedef struct 
{
    // Origin and radius
    float center_x, center_y, center_z;
    float radius;
} Sphere;

// Intersection data
typedef struct
{
    // Hit point
    float hit_point_x, hit_point_y, hit_point_z;
    // Normal
    float normal_x, normal_y, normal_z;
    // UV coordinates
    float uv_s, uv_t;
} Intersection;

// Intersect ray with sphere
inline bool IntersectRaySphere(const Sphere sphere, 
                               float ray_origin_x, float ray_origin_y, float ray_origin_z,
                               float ray_direction_x, float ray_direction_y, float ray_direction_z,
                               float* ray_extent)
{
    // Compute terms for quadratic form
    const float center_origin_x = ray_origin_x - sphere.center_x;
    const float center_origin_y = ray_origin_y - sphere.center_y;
    const float center_origin_z = ray_origin_z - sphere.center_z;
    const float a = ray_direction_x * ray_direction_x + 
                    ray_direction_y * ray_direction_y +
                    ray_direction_z * ray_direction_z;
    const float b = 2.f * (ray_direction_x * center_origin_x + 
                           ray_direction_y * center_origin_y +
                           ray_direction_z * center_origin_z);
    const float c = center_origin_x * center_origin_x +
                    center_origin_y * center_origin_y +
                    center_origin_z * center_origin_z -
                    sphere.radius * sphere.radius;

    // Solve to check if we intersect the sphere
    float t0, t1;
    if (SolveQuadratic(a, b, c, &t0, &t1))
    {
        return false;
    }

    // Select proper intersection parameter and update ray if we have an hit
    if (t0 > *ray_extent || t1 < 0.f)
    {
        return false;
    }

    float t_hit = t0;
    if (t_hit < 0.f)
    {
        t_hit = t1;
        if (t_hit > *ray_extent)
        {
            return false;
        }
    }

    // If we get here, we have a hit, udpate ray extent
    *ray_extent = t_hit;

    return true;
}

// Small tollerance one for degenerate uv coordinates
#define EPS 0.0001f

// Fill intersection for given ray and intersection parameter
inline Intersection FillIntersection(const Sphere sphere,
                                     float ray_origin_x, float ray_origin_y, float ray_origin_z,
                                     float ray_direction_x, float ray_direction_y, float ray_direction_z,
                                     float t_hit)
{
    Intersection isect;

    isect.hit_point_x = ray_origin_x + t_hit * ray_direction_x;
    isect.hit_point_y = ray_origin_y + t_hit * ray_direction_y;
    isect.hit_point_z = ray_origin_z + t_hit * ray_direction_z;

    const float inv_radius = 1.f / sphere.radius;
    isect.normal_x = (isect.hit_point_x - sphere.center_x) * inv_radius;
    isect.normal_y = (isect.hit_point_y - sphere.center_y) * inv_radius;
    isect.normal_z = (isect.hit_point_z - sphere.center_z) * inv_radius;

    // Compute UVs
    if (fabs(isect.hit_point_x) < EPS && fabs(isect.hit_point_z) < EPS)
    {
        isect.uv_s = 0.f;
        isect.uv_t = (isect.hit_point_y - sphere.center_y) > 0.f ? 0.f : 1.f;
    }
    else
    {   
        float phi = atan2(isect.hit_point_z, isect.hit_point_x);
        if (phi < 0.f)
        {
            phi += 2.f * M_PI_F;
        }
        isect.uv_s = phi * 0.5f * M_1_PI_F;
        isect.uv_t = acos(isect.hit_point_y / sphere.radius) * M_1_PI_F;
    }

    return isect;
}

#endif // SPHERE_H
