//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_CAMERA_HPP
#define RABBIT_CAMERA_HPP

#include <cl2.hpp>
#include "Vector.hpp"

namespace Rendering
{

class Camera
{
public:
    // Setup new camera in a Look-At way
    Camera(const Vector3& eye, const Vector3& at, const Vector3& up,
           float fov, unsigned int width, unsigned int height) noexcept;

    // Move camera around
    void Move(const Vector3& eye, const Vector3& at, const Vector3& up) noexcept;

private:
    // Compute local base
    void ComputeLocalBase(const Vector3& eye, const Vector3& at, const Vector3& up) noexcept;

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
