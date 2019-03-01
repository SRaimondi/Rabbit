//
// Created by Simon on 2019-03-01.
//

#include "Camera.hpp"
#include "Common.hpp"

namespace Rendering
{

Camera::Camera(const Vector3& eye, const Vector3& at, const Vector3& up, float fov, unsigned int width,
               unsigned int height) noexcept
    : eye{ eye }, image_width{ width }, image_height{ height },
      inv_width{ 1.f / image_width }, inv_height{ 1.f / image_height }
{
    // Compute local base
    w = Normalize(eye - at);
    u = Normalize(Cross(up, w));
    v = Cross(w, u);
    // Compute FOV
    bottom = -std::tan(Radians(fov / 2.f));
    left = bottom * image_width / image_height;
}

} // Rendering namespace