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
    // Center
    float cx, cy, cz;
    // Radius
    float radius;

    constexpr Sphere(float cx, float cy, float cz, float r) noexcept
        : cx{ cx }, cy{ cy }, cz{ cz }, radius{ r }
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

    SceneDescription();
};

class SceneParser
{
public:
    // Read file and create SceneDescription
    static SceneDescription ReadSceneDescription(const std::string& filename);
};

#endif //RABBIT_SCENEPARSER_HPP
