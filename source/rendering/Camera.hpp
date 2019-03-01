//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_CAMERA_HPP
#define RABBIT_CAMERA_HPP

#include "Vector.hpp"

namespace Rendering
{

struct Camera
{
    // Setup new camera in a Look-At methods
    Camera(const Vector3& eye, const Vector3& at, const Vector3& up,
           float fov, unsigned int width, unsigned int height) noexcept;

    // Eye position
    Vector3 eye;
    // Local base
    Vector3 u, v, w;
    // FOV
    float bottom, left;
    // Image size
    const unsigned int image_width, image_height;
    // Inverse of the size of the image
    const float inv_width, inv_height;
};

} // Rendering namespace

#endif //RABBIT_CAMERA_HPP
