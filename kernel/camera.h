#ifndef CAMERA_H
#define CAMERA_H

// Camera data layout as in source/Camera.hpp
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

// Generate ray for the given pixel and sample offset
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

#endif // CAMERA_H