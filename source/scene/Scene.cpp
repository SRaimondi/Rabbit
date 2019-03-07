//
// Created by simon on 3/7/2019.
//

#include "Scene.hpp"

#include <iostream>

namespace CL
{

Scene::Scene(cl_context context, const SceneDescription& scene_description, const ::Rendering::Camera& camera)
    : d_spheres{ nullptr }, num_spheres{ static_cast<unsigned int>(scene_description.loaded_spheres.size()) },
      d_camera{ nullptr }
{
    cl_int err_code{ CL_SUCCESS };

    try
    {
        d_spheres = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, num_spheres * sizeof(Sphere),
                                   const_cast<Sphere*>(scene_description.loaded_spheres.data()), &err_code);
        CL_CHECK_STATUS(err_code);
        d_camera = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(::Rendering::Camera),
                                  const_cast<::Rendering::Camera*>(&camera), &err_code);
        CL_CHECK_STATUS(err_code);
    }
    catch (const std::exception& ex)
    {
        // Cleanup what is needed and rethrow exception
        Cleanup();
        throw;
    }
}

Scene::~Scene() noexcept
{
    Cleanup();
}

void Scene::Cleanup() noexcept
{
    try
    {
        if (d_spheres != nullptr)
        {
            CL_CHECK_CALL(clReleaseMemObject(d_spheres));
        }
        if (d_camera != nullptr)
        {
            CL_CHECK_CALL(clReleaseMemObject(d_camera));
        }
    }
    catch (const std::exception& ex)
    {
        // TODO operator<< could throw
        std::cerr << ex.what() << std::endl;
    }
}

} // CL namespace