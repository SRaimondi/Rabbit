//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_VECTOR_HPP
#define RABBIT_VECTOR_HPP

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

constexpr const Vector3 Cross(const Vector3& lhs, const Vector3& rhs) noexcept
{
    return {}
}

#endif //RABBIT_VECTOR_HPP
