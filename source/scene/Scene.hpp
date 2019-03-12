//
// Created by simon on 3/7/2019.
//

#ifndef RABBIT_SCENE_HPP
#define RABBIT_SCENE_HPP

#include "CLError.hpp"
#include "SceneParser.hpp"
#include "Camera.hpp"

namespace CL
{

// Spheres and camera storage for the device
class Scene
{
public:
    Scene(cl_context context, const SceneDescription& scene_description, const ::Rendering::Camera& camera);

    ~Scene() noexcept;

    // List of spheres
    cl_mem d_spheres;
    const cl_uint num_spheres;

    // List of indices of material for each sphere
    cl_mem d_material_indices;

    // List of materials
    cl_mem d_materials;

    // Camera on the device
    cl_mem d_camera;

private:
    // Cleanup all buffers without throwing
    void Cleanup() noexcept;
};

} // CL namespace

#endif //RABBIT_SCENE_HPP
