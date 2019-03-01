//
// Created by Simon on 2019-03-01.
//

#include "Scene.hpp"

Scene::Scene(const cl::Context& context, const std::vector<Sphere>& spheres) noexcept
    : h_spheres{ spheres }
{}
