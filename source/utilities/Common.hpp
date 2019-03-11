//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_COMMON_HPP
#define RABBIT_COMMON_HPP

#include <type_traits>

// Constants
template <typename T>
constexpr T PI{ 3.1415926535897932385L };

template <typename T>
constexpr T TWO_PI{ T(2) * PI<T> };

template <typename T>
constexpr T PI_2{ PI<T> / T(2) };

template <typename T>
constexpr T PI_4{ PI<T> / T(4) };

template <typename T>
constexpr T ONE_OVER_PI{ T(1) / PI<T> };

template <typename T>
constexpr T ONE_OVER_2PI{ T(1) / TWO_PI<T> };

// Convert radians to degrees
template <typename T, typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
constexpr T Radians(const T& deg) noexcept
{
    return deg * (PI<T> / T(180));
}

// Convert degrees to radians
template <typename T, typename std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
constexpr T Degrees(const T& rad) noexcept
{
    return rad * (T(180) / PI<T>);
}

// Divide two values up to the next closest integer value
template <typename T, typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
constexpr T DivideUp(const T& a, const T& b) noexcept
{
    return (a + b - 1) / b;
}

// Divide two values down to the next closest integer value
template <typename T, typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
constexpr T DivideDown(const T& a, const T& b) noexcept
{
    return a / b;
}

// Round up the first value to the closest multiple of the second one
template <typename T, typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
constexpr T RoundUp(const T& a, const T& b) noexcept
{
    return DivideUp(a, b) * b;
}

// Round down the first value to the closest multiple of the second one
template <typename T, typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
constexpr T RoundDown(const T& a, const T& b) noexcept
{
    return DivideDown(a, b) * b;
}

#endif //RABBIT_COMMON_HPP
