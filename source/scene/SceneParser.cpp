//
// Created by Simon on 2019-03-01.
//

#include "SceneParser.hpp"

#include <fstream>
#include <sstream>

SceneDescription::SceneDescription()
    : image_width{ 0 }, image_height{ 0 }, tile_width{ 0 }, tile_height{ 0 }, pixel_samples{ 0 }
{}

SceneDescription SceneParser::ReadSceneDescription(const std::string& filename)
{
    SceneDescription scene_description;

    // Open file for reading
    std::ifstream scene_file{ filename };
    if (!scene_file.is_open())
    {
        std::ostringstream error_message;
        error_message << "Could not open file: " << filename;
        throw std::invalid_argument{ error_message.str() };
    }

    // TODO There is no check on the scene format for the moment

    // Read image size, tile size and samples per pixel
    scene_file >> scene_description.image_width >> scene_description.image_height;
    scene_file >> scene_description.tile_width >> scene_description.tile_height;
    scene_file >> scene_description.pixel_samples;

    // Read materials
    unsigned int num_materials;
    scene_file >> num_materials;
    for (unsigned int m = 0; m != num_materials; m++)
    {
        float rho_r, rho_g, rho_b, emission_r, emission_g, emission_b;
        scene_file >> rho_r >> rho_g >> rho_b >> emission_r >> emission_g >> emission_b;
        scene_description.loaded_materials.emplace_back(rho_r, rho_g, rho_b, emission_r, emission_g, emission_b);
    }

    // Read how many spheres are in the scene
    unsigned int num_spheres;
    scene_file >> num_spheres;

    // Load and add each sphere to the scene
    for (unsigned int s = 0; s != num_spheres; s++)
    {
        float cx, cy, cz, r;
        unsigned int material_index;
        scene_file >> cx >> cy >> cz >> r >> material_index;
        scene_description.loaded_spheres.emplace_back(cx, cy, cz, r);
        scene_description.material_index.emplace_back(material_index);
    }

    return scene_description;
}
