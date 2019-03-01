//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_VECTOR_HPP
#define RABBIT_VECTOR_HPP

#include <cmath>

// A very simple 3D vector implementation used on the host for geometry computations
struct Vector3
{
    // Vector components
    float x, y, z;

    constexpr Vector3() noexcept
        : x{ 0.f }, y{ 0.f }, z{ 0.f }
    {}

    constexpr explicit Vector3(float v) noexcept
        : x{ v }, y{ v }, z{ v }
    {}

    constexpr Vector3(float x, float y, float z) noexcept
        : x{ x }, y{ y }, z{ z }
    {}
};

// Math operations
constexpr const Vector3 operator+(const Vector3& lhs, const Vector3& rhs) noexcept
{
    return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.x + rhs.y };
}

constexpr const Vector3 operator-(const Vector3& lhs, const Vector3& rhs) noexcept
{
    return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.x - rhs.y };
}

constexpr const Vector3 operator*(float lhs, const Vector3& rhs) noexcept
{
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}

constexpr const Vector3 operator*(const Vector3& lhs, float rhs) noexcept
{
    return {lhs.x * rhs, lhs.y * rhs, lhs.z * rhs};
}

constexpr const Vector3 operator/(const Vector3& lhs, float rhs) noexcept
{
    const float inv_rhs = 1.f / rhs;
    return inv_rhs * lhs;
}

constexpr const Vector3 Cross(const Vector3& lhs, const Vector3& rhs) noexcept
{
    return { lhs.y * rhs.z - lhs.z * rhs.y,
             lhs.z * rhs.x - lhs.x * rhs.z,
             lhs.x * rhs.y - lhs.y * rhs.x };
}

constexpr float Dot(const Vector3& lhs, const Vector3& rhs) noexcept
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

constexpr float SquaredNorm(const Vector3& v) noexcept
{
    return Dot(v, v);
}

inline float Norm(const Vector3& v) noexcept
{
    return std::sqrt(SquaredNorm(v));
}

inline const Vector3 Normalize(const Vector3& v) noexcept
{
    return v / Norm(v);
}

#endif //RABBIT_VECTOR_HPP
