//
// Created by Simon on 2019-03-01.
//

#include "Scene.hpp"

Scene::Scene(const cl::Context& context, const std::vector<Sphere>& spheres) noexcept
    : h_spheres{ spheres },
      d_spheres{ context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, spheres.size() * sizeof(Sphere), h_spheres.data() }
{}
