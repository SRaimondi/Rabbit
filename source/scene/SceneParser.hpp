//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_SCENEPARSER_HPP
#define RABBIT_SCENEPARSER_HPP

#include <vector>
#include <string>

// Sphere struct layout
struct Sphere
{
    // Center and radius
    float cx, cy, cz, radius;

    constexpr Sphere(float cx, float cy, float cz, float r) noexcept
        : cx{ cx }, cy{ cy }, cz{ cz }, radius{ r }
    {}
};

// Material struct layout
struct DiffuseMaterial
{
    // Reflectance
    float rho_r, rho_g, rho_b;
    // Emission
    float emission_r, emission_g, emission_b;

    constexpr DiffuseMaterial(float rr, float rg, float rb,
                              float er, float eg, float eb) noexcept
        : rho_r{ rr }, rho_g{ rg }, rho_b{ rb },
          emission_r{ er }, emission_g{ eg }, emission_b{ eb }
    {}
};

// Utility class that parses the given file and returns a SceneDescription object
struct SceneDescription
{
    // Image size
    unsigned int image_width, image_height;
    // Tile size
    unsigned int tile_width, tile_height;
    // Samples per-pixel
    unsigned int pixel_samples;

    // Spheres in the scene
    std::vector<Sphere> loaded_spheres;
    // Indices of the materials
    std::vector<unsigned int> material_index;
    // Materials
    std::vector<DiffuseMaterial> loaded_materials;

    SceneDescription();

    unsigned int NumSpheres() const noexcept
    {
        return static_cast<unsigned int>(loaded_spheres.size());
    }

    unsigned int NumMaterials() const noexcept
    {
        return static_cast<unsigned int>(loaded_materials.size());
    }
};

class SceneParser
{
public:
    // Read file and create SceneDescription
    static SceneDescription ReadSceneDescription(const std::string& filename);
};

#endif //RABBIT_SCENEPARSER_HPP
