/*
 * Constants for kernels
 */

#define MAX_UINT                4294967295u
#define INVALID_PRIM_INDEX      MAX_UINT
#define RAY_DONE_DEPTH          MAX_UINT
#define XORSHIFT_STATE_START    52938u
#define TWO_PI                  6.28318530718f
#define ONE_OVER_2PI            0.15915494309f
#define EPS                     0.0001f

/*
 * Common utility functions
 */

inline bool SolveQuadratic(float a, float b, float c, float* t0, float* t1)
{
    const float discr = b * b - 4.f * a * c;
    if (discr < 0.f)
    {
        return false;
    }

    const float discr_root = sqrt(discr);
    float q;
    if (b < 0.f)
    {
        q = -0.5f * (b - discr_root);
    }
    else
    {
        q = -0.5f * (b + discr_root);
    }

    *t0 = q / a;
    *t1 = c / q;

    if (*t0 > *t1)
    {
        const float temp = *t0;
        *t0 = *t1;
        *t1 = temp;
    }

    return true;
}

/*
 * Camera struct and function to generate a ray for a given pixel
 */

typedef struct 
{
    // Eye position
    float eye_x, eye_y, eye_z;
    // Local base
    float u_x, u_y, u_z;
    float v_x, v_y, v_z;
    float w_x, w_y, w_z;
    // FOV
    float bottom, left;
    // Image size
    const unsigned int image_width, image_height;
    // Inverse of the size of the image
    const float inv_width, inv_height;
} Camera;

inline float3 GenerateRayDirection(__constant const Camera* camera,
                                   unsigned int px, unsigned int py,
                                   float sx, float sy)
{
    const float vp_x = camera->left * (1.f - 2.f * (px + sx) * camera->inv_width);
    const float vp_y = camera->bottom * (1.f - 2.f * (py + sy) * camera->inv_height);

    // Load local base
    const float3 u = (float3)(camera->u_x, camera->u_y, camera->u_z);
    const float3 v = (float3)(camera->v_x, camera->v_y, camera->v_z);
    const float3 w = (float3)(camera->w_x, camera->w_y, camera->w_z);

    // Compute direction
    return normalize(vp_x * u + vp_y * v - w);
}

/*
 * Sphere struct and functions to intersect a sphere with a ray and filling an intersection
 */

typedef struct 
{
    // Origin and radius
    float center_x, center_y, center_z;
    float radius;
} Sphere;

typedef struct
{
    // Hit point
    float hit_point_x, hit_point_y, hit_point_z;
    // Normal
    float normal_x, normal_y, normal_z;
    // UV coordinates
    float uv_s, uv_t;
} Intersection;

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
            phi += TWO_PI;
        }
        isect.uv_s = phi * ONE_OVER_2PI;
        isect.uv_t = acos(isect.hit_point_y / sphere.radius) * M_1_PI_F;
    }

    return isect;
}

/*
 * Random number generation
 */
typedef union
{
    unsigned int ui;
    float f;
} RandomFloatHelper;

inline float GenerateFloat(__global unsigned int* xorshift_state)
{
    // Generate new state
    unsigned int new_state = *xorshift_state;
    new_state ^= new_state << 13;
    new_state ^= new_state >> 17;
    new_state ^= new_state << 5;

    // Store new state
    *xorshift_state = new_state;

    // Compute float value
    RandomFloatHelper temp;
    temp.ui = (new_state >> 9) | 0x3f800000u;

    return temp.f;
}

