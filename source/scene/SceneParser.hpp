//
// Created by Simon on 2019-03-01.
//

#ifndef RABBIT_SCENEPARSER_HPP
#define RABBIT_SCENEPARSER_HPP

#include "Scene.hpp"

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

    // Generate Scene from a loaded scene description
    static Scene CreateScene(const cl::Context& context, const SceneDescription& scene_description);
};

#endif //RABBIT_SCENEPARSER_HPP
