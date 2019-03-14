/*
 * Constants for kernels
 */
#define MAX_UINT                4294967295u

#define INVALID_PRIM_INDEX      MAX_UINT

#define RAY_FIRST_TILE_DEPTH    4294967293u
#define RAY_TO_RESTART_DEPTH    4294967294u
#define RAY_DONE_DEPTH          4294967295u

#define XORSHIFT_STATE_START    5293385u
#define XORSHIFT_STATE_MULT     68435u

#define TWO_PI                  6.28318530718f
#define ONE_OVER_2PI            0.15915494309f
#define EPS                     0.0001f

/*
 * 2D / 3D vector struct
 */
typedef struct
{
    float x, y;
} Vector2;

inline Vector2 NewVector2(float x, float y)
{
    return (Vector2){ .x = x, .y = y };
}

typedef struct 
{
    float x, y, z;
} Vector3;

inline Vector3 NewVector3(float x, float y, float z)
{
    return (Vector3){ .x = x, .y = y, .z = z };
}

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

inline void CreateLocalBase(Vector3 n, Vector3* s, Vector3* t)
{
    if (fabs(n.x) > fabs(n.y)) {
        const float inv_norm = 1.f / sqrt(n.x * n.x + n.z * n.z);
        s->x = -n.z * inv_norm;
        s->y = 0.f;
        s->z = n.x * inv_norm;
    } else {
        const float inv_norm = 1.f / sqrt(n.y * n.y + n.z * n.z);
        s->x = 0.f;
        s->y = n.z * inv_norm;
        s->z = -n.y * inv_norm;
    }
    t->x = s->y * n.z - s->z * n.y;
    t->y = s->z * n.x - s->x * n.z;
    t->z = s->x * n.y - s->y * n.x;
}

inline Vector2 ConcentricSampleDisk(float u0, float u1)
{
    // Map points to [-1, 1]
    float u0_offset = 2.f * u0 - 1.f;
    float u1_offset = 2.f * u1 - 1.f;

    // Handle degenerate center
    if (fabs(u0_offset) < EPS && fabs(u1_offset) < EPS)
    {
        return NewVector2(0.f, 0.f);
    }

    // Apply concentric mapping to point
    float theta, r;
    if (fabs(u0_offset) > fabs(u1_offset))
    {
        r = u0_offset;
        theta = M_PI_4_F * (u1_offset / u0_offset);
    }
    else
    {
        r = u1_offset;
        theta = M_PI_2_F - M_PI_4_F * (u0_offset / u1_offset);
    }

    return NewVector2(r * cos(theta), r * sin(theta));
}

inline Vector3 CosineSampleHemisphere(float u0, float u1)
{
    const Vector2 d = ConcentricSampleDisk(u0, u1);
    const float y = sqrt(fmax(0.f, 1.f - d.x * d.x - d.y * d.y));

    return NewVector3(d.x, y, d.y);
}

inline float CosineSampleHemispherePdf(float cos_theta)
{
    return cos_theta * M_1_PI_F;
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

inline Vector3 GenerateRayDirection(__constant const Camera* camera,
                                   unsigned int px, unsigned int py,
                                   float sx, float sy)
{
    const float vp_x = camera->left * (1.f - 2.f * (px + sx) * camera->inv_width);
    const float vp_y = camera->bottom * (1.f - 2.f * (py + sy) * camera->inv_height);

    // Compute direction
    const float dir_x = vp_x * camera->u_x + vp_y * camera->v_x - camera->w_x;
    const float dir_y = vp_x * camera->u_y + vp_y * camera->v_y - camera->w_y;
    const float dir_z = vp_x * camera->u_z + vp_y * camera->v_z - camera->w_z;
    const float inv_norm = 1.f / sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);

    return (Vector3){ .x = dir_x * inv_norm, .y = dir_y * inv_norm, .z = dir_z * inv_norm };
}

/*
 * Sphere struct and functions to intersect a sphere with a ray and filling an intersection
 */
typedef struct 
{
    // Origin and radius
    float center_x, center_y, center_z, radius;
} Sphere;

/*
 * Diffuse material description
 */ 
typedef struct
{
    // Reflectance
    float rho_r, rho_g, rho_b;
    // Emission
    float emission_r, emission_g, emission_b;
} DiffuseMaterial;

inline bool IsBlack(float r, float g, float b)
{
    return r == 0.f && g == 0.f && b == 0.f;
}

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
    if (!SolveQuadratic(a, b, c, &t0, &t1))
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

inline unsigned int NextUInt32(__global unsigned int* xorshift_state)
{
    // Generate new state
    unsigned int new_state = *xorshift_state;
    new_state ^= new_state << 13;
    new_state ^= new_state >> 17;
    new_state ^= new_state << 5;

    // Store new state
    *xorshift_state = new_state;

    return new_state;
}

inline float GenerateFloat(__global unsigned int* xorshift_state)
{
    // Compute float value
    RandomFloatHelper temp;
    temp.ui = (NextUInt32(xorshift_state) >> 9) | 0x3f800000u;

    return temp.f - 1.f;
}

/*
 * Initialise kernel only sets the ray depth to DONE and the seed for the random number generation
 */
__kernel void Initialise(// The ray depth is set to RAY_FIRST_TILE_DEPTH so the Restart kernel sets it to the proper values for the first tile
                         __global unsigned int* ray_depth,
                         // XOrsShift state
                         __global unsigned int* xorshift_state,
                         // Total number of samples
                         unsigned int total_samples)
{
    const unsigned int tid = get_global_id(0);
    if (tid < total_samples)
    {
        // Ray depth is set such that the first Restart sets them up
        ray_depth[tid] = RAY_FIRST_TILE_DEPTH;

        // Initialise xorshift random number generator state
        unsigned int mult = 1;
        unsigned int xorshift_init_state = 0;
        do
        {
            xorshift_init_state = XORSHIFT_STATE_START + mult++ * XORSHIFT_STATE_MULT * tid;
        } while (xorshift_init_state == 0);
        xorshift_state[tid] = xorshift_init_state;
        (void)NextUInt32(&xorshift_state[tid]);
    }
}

/*
 * Restart samples kernel, last flag is used to tell the kernel if this is the first tile or not
 */
__kernel void RestartSample(__constant const Camera* camera,
                            // Rays description
                            __global float* ray_origin_x, __global float* ray_origin_y, __global float* ray_origin_z,
                            __global float* ray_direction_x, __global float* ray_direction_y, __global float* ray_direction_z, 
                            __global unsigned int* ray_depth,
                            // Index of the primitive hit
                            __global unsigned int* primitive_index,
                            // Samples description
                            __global float* Li_r, __global float* Li_g, __global float* Li_b,
                            __global float* beta_r, __global float* beta_g, __global float* beta_b,
                            __global unsigned int* pixel_x, __global unsigned int* pixel_y,
                            __global float* sample_offset_x, __global float* sample_offset_y,
                            // Pixel where the sample deposit their computed radiance value
                            __global float* pixel_r, __global float* pixel_g, __global float* pixel_b,
                            __global float* filter_weight,
                            // Random number generator state
                            __global unsigned int* xorshift_state,
                            // Description of the tile
                            unsigned int tile_width, unsigned int tile_height,
                            unsigned int samples_per_pixel,
                            // Number of samples done
                            __global unsigned int* samples_done)
{
    const unsigned int tid = get_global_id(0);
    // Check if we need to restart this ray or not
    if (tid < tile_width * tile_height * samples_per_pixel)
    {
        const unsigned int current_ray_depth = ray_depth[tid];
        if (current_ray_depth != RAY_DONE_DEPTH &&
            (current_ray_depth == RAY_TO_RESTART_DEPTH || current_ray_depth == RAY_FIRST_TILE_DEPTH))
        {
            // Compute the coordinates of the pixel in the tile for the sample
            const unsigned int linear_pixel_index = tid / samples_per_pixel;
            const unsigned int tile_y = linear_pixel_index / tile_width;
            const unsigned int tile_x = linear_pixel_index - tile_y * tile_width;

            unsigned int px, py;
            // Check if this is the first tile or not
            if (current_ray_depth == RAY_FIRST_TILE_DEPTH) 
            {
                px = tile_x;
                py = tile_y;
            }
            else 
            {
                const unsigned int current_pixel_x = pixel_x[tid];
                const unsigned int current_pixel_y = pixel_y[tid];
                // Check if the sample's pixel is in the right next tile
                if (current_pixel_x + tile_width < camera->image_width)
                {
                    // Only update pixel x coordinate
                    px = current_pixel_x + tile_width;
                }
                else 
                {
                    // Check if we can go up
                    if (current_pixel_y + tile_height < camera->image_height)
                    {
                        // We went up, update pixel y and x
                        px = tile_x;
                        py = current_pixel_y + tile_height;
                    }
                    else
                    {
                        // The sample is done, we set the pixel coordinate to the image size so we take it into account
                        px = camera->image_width;
                        py = camera->image_height;
                    }
                }
            }

            // Check we are inside the image
            if (px < camera->image_width && py < camera->image_height)
            {
                // Reset samples' accumulated radiance
                Li_r[tid] = 0.f;
                Li_g[tid] = 0.f;
                Li_b[tid] = 0.f;

                beta_r[tid] = 1.f;
                beta_g[tid] = 1.f;
                beta_b[tid] = 1.f;

                // Store
                pixel_x[tid] = px;
                pixel_y[tid] = py;

                // Generate a random offset in the pixel for each sample
                // This is pure random now, next would be to stratify the samples
                const float sx = GenerateFloat(&xorshift_state[tid]);
                const float sy = GenerateFloat(&xorshift_state[tid]);

                // Store
                sample_offset_x[tid] = sx;
                sample_offset_y[tid] = sy;

                // Setup the rays for each sample
                ray_origin_x[tid] = camera->eye_x;
                ray_origin_y[tid] = camera->eye_y;
                ray_origin_z[tid] = camera->eye_z;

                // Generate direction and store
                const Vector3 ray_direction = GenerateRayDirection(camera, px, py, sx, sy);
                ray_direction_x[tid] = ray_direction.x;
                ray_direction_y[tid] = ray_direction.y;
                ray_direction_z[tid] = ray_direction.z;

                // Reset depth
                ray_depth[tid] = 0;

                // Reset primitive index
                primitive_index[tid] = INVALID_PRIM_INDEX;

                // If the thread id is the first sample in the pixel, reset the raster values
                if (tid % samples_per_pixel == 0)
                {
                    pixel_r[tid] = 0.f;
                    pixel_g[tid] = 0.f;
                    pixel_b[tid] = 0.f;
                    filter_weight[tid] = 0.f;
                }
            }
            else
            {
                // The sample is done, set depth and increase count
                ray_depth[tid] = RAY_DONE_DEPTH;
                (void)atomic_add(samples_done, 1);
            }
        }
    }
}

/*
 * Intersect kernel
 */
__kernel void Intersect(// Spheres in the scene
                        __global const Sphere* spheres, unsigned int num_spheres,
                        // Ray origin and direction
                        __global const float* ray_origin_x, __global const float* ray_origin_y, __global const float* ray_origin_z,
                        __global const float* ray_direction_x, __global const float* ray_direction_y, __global const float* ray_direction_z,
                        __global unsigned int* ray_depth,
                        // Intersection information
                        __global float* hit_point_x, __global float* hit_point_y, __global float* hit_point_z,
                        __global float* normal_x, __global float* normal_y, __global float* normal_z,
                        __global float* uv_s, __global float* uv_t, 
                        __global float* wo_x, __global float* wo_y, __global float* wo_z,
                        __global unsigned int* primitive_index,
                        // Total number of samples
                        unsigned int total_samples)
{
    const unsigned int tid = get_global_id(0);
    if (tid < total_samples && ray_depth[tid] != RAY_DONE_DEPTH)
    {
        // Load ray data
        const float ox = ray_origin_x[tid];
        const float oy = ray_origin_y[tid];
        const float oz = ray_origin_z[tid];
        const float dx = ray_direction_x[tid];
        const float dy = ray_direction_y[tid];
        const float dz = ray_direction_z[tid];
        float extent = MAXFLOAT;

        // Intersect ray with spheres
        unsigned int closest_sphere_index = num_spheres;
        Sphere closest_sphere;
        for (unsigned int s = 0; s != num_spheres; s++)
        {
            if (IntersectRaySphere(spheres[s], ox, oy, oz, dx, dy, dz, &extent))
            {
                closest_sphere_index = s;
                closest_sphere = spheres[closest_sphere_index];
            }
        }

        if (closest_sphere_index != num_spheres)
        {
            // Compute intersection and store
            const Intersection intersection = FillIntersection(closest_sphere, ox, oy, oz, dx, dy, dz, extent);
            hit_point_x[tid] = intersection.hit_point_x;
            hit_point_y[tid] = intersection.hit_point_y;
            hit_point_z[tid] = intersection.hit_point_z;
            normal_x[tid] = intersection.normal_x;
            normal_y[tid] = intersection.normal_y;
            normal_z[tid] = intersection.normal_z;
            uv_s[tid] = intersection.uv_s;
            uv_t[tid] = intersection.uv_t;
            wo_x[tid] = -dx;
            wo_y[tid] = -dy;
            wo_z[tid] = -dz;

            // Save index of primitive hit
            primitive_index[tid] = closest_sphere_index;
        }
        else
        {
            // Ray did not intersect, set for restart
            ray_depth[tid] = RAY_TO_RESTART_DEPTH;
        }
    }
}

/*
 * This kernel checks if the ray is not done abd sets up a new ray forthe next bounce
 */
__kernel void SampleBRDF(// Rays description
                         __global float* ray_origin_x, __global float* ray_origin_y, __global float* ray_origin_z,
                         __global float* ray_direction_x, __global float* ray_direction_y, __global float* ray_direction_z, 
                         __global unsigned int* ray_depth,
                         // Intersection information
                         __global const float* hit_point_x, __global const float* hit_point_y, __global const float* hit_point_z,
                         __global const float* normal_x, __global const float* normal_y, __global const float* normal_z,
                         // Random number generator state
                         __global unsigned int* xorshift_state,
                         // Total number of samples
                         unsigned int total_samples)
{
    const unsigned int tid = get_global_id(0);
    if (tid < total_samples && ray_depth[tid] != RAY_TO_RESTART_DEPTH && ray_depth[tid] != RAY_DONE_DEPTH)
    {
        // Create local base around normal
        const Vector3 n = NewVector3(normal_x[tid], normal_y[tid], normal_z[tid]);
        Vector3 s, t;
        CreateLocalBase(n, &s, &t);

        // Sample random cosine-weighted direction
        const float u0 = GenerateFloat(&xorshift_state[tid]);
        const float u1 = GenerateFloat(&xorshift_state[tid]);
        const Vector3 wi = CosineSampleHemisphere(u0, u1);

        // Transform direction to world space
        const Vector3 wi_world = NewVector3(wi.x * s.x + wi.y * n.x + wi.z * t.x,
                                            wi.x * s.y + wi.y * n.y + wi.z * t.y,
                                            wi.x * s.z + wi.y * n.z + wi.z * t.z);

        // Set new ray start
        ray_origin_x[tid] = hit_point_x[tid] + EPS * wi_world.x;
        ray_origin_y[tid] = hit_point_y[tid] + EPS * wi_world.y;
        ray_origin_z[tid] = hit_point_z[tid] + EPS * wi_world.z;

        // Set new ray direction
        ray_direction_x[tid] = wi_world.x;
        ray_direction_y[tid] = wi_world.y;
        ray_direction_z[tid] = wi_world.z;

        // Increase depth
        ray_depth[tid]++;
    }
}

/*
 * Update sample radiance
 */
__kernel void UpdateRadiance(// Current radiance along the ray and masking term
                             __global float* Li_r, __global float* Li_g, __global float* Li_b,
                             __global float* beta_r, __global float* beta_g, __global float* beta_b,
                             // Intersection information
                             __global const float* hit_point_x, __global const float* hit_point_y, __global const float* hit_point_z,
                             __global const float* normal_x, __global const float* normal_y, __global const float* normal_z,
                             __global const float* uv_s, __global const float* uv_t,
                             __global const unsigned int* primitive_index,
                             // Next ray direction
                             __global const float* ray_direction_x, __global const float* ray_direction_y, __global const float* ray_direction_z)
{

}

/*
 * Deposit samples on raster kernel
 */
__kernel void DepositSamples()
{
    return;
}