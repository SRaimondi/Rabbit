/*
 * Constants for kernels
 */
#define MAX_UINT                4294967295u

#define INVALID_PRIM_INDEX      MAX_UINT

#define RAY_FIRST_TILE_DEPTH    4294967293u
#define RAY_TO_RESTART_DEPTH    4294967294u
#define RAY_DONE_DEPTH          4294967295u

#define XORSHIFT_STATE_START    5293385u

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
    float center_x, center_y, center_z, radius;
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
        ray_depth[tid] = RAY_FIRST_TILE_DEPTH;
        xorshift_state[tid] = XORSHIFT_STATE_START + tid;
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
                py = tile_y;
                px = tile_x;
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
                    if (current_pixel_y + tile_height < camera->image_height) {
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
                const float sx = GenerateFloat(xorshift_state + tid);
                const float sy = GenerateFloat(xorshift_state + tid);

                // Store
                sample_offset_x[tid] = sx;
                sample_offset_y[tid] = sy;

                // Setup the rays for each sample
                ray_origin_x[tid] = camera->eye_x;
                ray_origin_y[tid] = camera->eye_y;
                ray_origin_z[tid] = camera->eye_z;

                // Generate direction and store
                const float3 ray_direction = GenerateRayDirection(camera, px, py, sx, sy);
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
                        __global const unsigned int* ray_depth,
                        // Intersection information
                        __global float* hit_point_x, __global float* hit_point_y, __global float* hit_point_z,
                        __global float* normal_x, __global float* normal_y, __global float* normal_z,
                        __global float* uv_s, __global float* uv_t, 
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
        unsigned int closest_sphere = num_spheres;
        for (unsigned int s = 0; s != num_spheres; s++)
        {
            if (IntersectRaySphere(spheres[s], ox, oy, oz, dx, dy, dz, &extent))
            {
                closest_sphere = s;
            }
        }

        if (closest_sphere < num_spheres)
        {
            // Compute intersection and store
            const Intersection intersection = FillIntersection(spheres[closest_sphere], ox, oy, oz, dx, dy, dz, extent);
            hit_point_x[tid] = intersection.hit_point_x;
            hit_point_y[tid] = intersection.hit_point_y;
            hit_point_z[tid] = intersection.hit_point_z;
            normal_x[tid] = intersection.normal_x;
            normal_y[tid] = intersection.normal_y;
            normal_z[tid] = intersection.normal_z;
            uv_s[tid] = intersection.uv_s;
            uv_t[tid] = intersection.uv_t;

            // Save index of primitive hit
            primitive_index[tid] = closest_sphere;
        }
    }
}

/*
 * Update sample radiance
 */
__kernel void UpdateRadiance()
{
    return;
}

/*
 * Deposit samples on raster kernel
 */
__kernel void DepositSamples()
{
    return;
}