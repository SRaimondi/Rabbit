//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_SCENE_HPP
#define RABBIT_SCENE_HPP

#include "OpenCLInclude.hpp"

// Sphere struct layout
struct Sphere
{
    // Center
    float cx, cy, cz;
    // Radius
    float radius;

    constexpr Sphere(float cx, float cy, float cz, float r) noexcept
        : cx{ cx }, cy{ cy }, cz{ cz }, radius{ r }
    {}
};

class Scene
{
public:
    // Create new scene given a list of spheres
    Scene(const cl::Context& context, const std::vector<Sphere>& spheres) noexcept;

private:
    // Host spheres
    std::vector<Sphere> h_spheres;

public:
    // Buffer with spheres on the device
    cl::Buffer d_spheres;
};

#endif //RABBIT_SCENE_HPP
