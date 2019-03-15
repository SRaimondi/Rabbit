#include "RenderingContext.hpp"
#include "TileRendering.hpp"
#include "CLError.hpp"

#include <array>
#include <iostream>
#include <memory>
#include <random>

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Invalid number of arguments, expecting file to Scene description file\n";
        exit(EXIT_FAILURE);
    }

    try
    {
        // Read scene description
        // const SceneDescription scene_description{ SceneParser::ReadSceneDescription(argv[1]) };

        // Random scene description
        SceneDescription scene_description;
        scene_description.image_width = 1920;
        scene_description.image_height = 1080;
        scene_description.tile_width = 32;
        scene_description.tile_height = 32;
        scene_description.pixel_samples = 1024;

        scene_description.loaded_spheres.emplace_back(0.f, -5000.f, 0.f, 5000.f);
        scene_description.loaded_materials.emplace_back(0.9f, 0.9f, 0.9f, 0.f, 0.f, 0.f);
        scene_description.material_index.push_back(0);

        scene_description.loaded_spheres.emplace_back(0.f, 0.f, 0.f, 10000.f);
        scene_description.loaded_materials.emplace_back(0.f, 0.f, 0.f, .6f, .6f, .6f);
        scene_description.material_index.push_back(1);

        std::mt19937 generator;
        std::uniform_real_distribution<float> position(-30.f, 30.f);
        std::uniform_real_distribution<float> radius(1.f, 4.f);
        std::uniform_real_distribution<float> color(0.2f, 0.99f);
        std::uniform_real_distribution<float> emitting;

        for (unsigned int s = 0; s != 70; s++)
        {
            const float r{ radius(generator) };
            scene_description.loaded_spheres.emplace_back(position(generator), r, position(generator), r);
            if (emitting(generator) < 0.2f)
            {
                scene_description.loaded_materials.emplace_back(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
            }
            else
            {
                scene_description.loaded_materials.emplace_back(color(generator), color(generator), color(generator), 0.f, 0.f, 0.f);
            }
            scene_description.material_index.push_back(scene_description.loaded_materials.size() - 1);
        }

        // Create camera
        const Rendering::Camera camera{ Vector3{ 10.f, 40.f, -60.f }, Vector3{ 0.f }, Vector3{ 0.f, 1.f, 0.f },
                                        45.f, scene_description.image_width, scene_description.image_height };

        cl_uint num_platforms;
        CL_CHECK_CALL(clGetPlatformIDs(0, nullptr, &num_platforms));
        if (num_platforms == 0)
        {
            throw std::runtime_error("No available OpenCL platforms");
        }
        std::vector<cl_platform_id> platforms(num_platforms);
        CL_CHECK_CALL(clGetPlatformIDs(num_platforms, platforms.data(), nullptr));

        // Select platform by name
        cl_platform_id selected_platform;
        if (num_platforms > 1)
        {
            std::size_t selected_platform_index{ num_platforms };
            std::cout << "Select platform by corresponding index\n";
            int platform_index{ 0 };
            for (auto platform : platforms)
            {
                size_t platform_name_size;
                CL_CHECK_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &platform_name_size));
                auto platform_name{ std::make_unique<char[]>(platform_name_size) };
                CL_CHECK_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, platform_name_size, platform_name.get(),
                                                nullptr));
                std::cout << "[" << platform_index++ << "]: " << platform_name.get() << "\n";
            }
            do
            {
                std::cout << "Platform index: ";
                std::cin >> selected_platform_index;
            }
            while (selected_platform_index >= num_platforms);
            selected_platform = platforms[selected_platform_index];
        }
        else
        {
            selected_platform = platforms[0];
            size_t platform_name_size;
            CL_CHECK_CALL(clGetPlatformInfo(selected_platform, CL_PLATFORM_NAME, 0, nullptr, &platform_name_size));
            auto selected_platform_name{ std::make_unique<char[]>(platform_name_size) };
            CL_CHECK_CALL(clGetPlatformInfo(selected_platform, CL_PLATFORM_NAME,
                                            platform_name_size, selected_platform_name.get(),
                                            nullptr));
            std::cout << "Found one platform: " << selected_platform_name.get() << "\n";
        }

        // Select device from platform
        cl_uint num_devices;
        CL_CHECK_CALL(clGetDeviceIDs(selected_platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &num_devices));
        if (num_devices == 0)
        {
            throw std::runtime_error("No available GPU devices in selected platform");
        }
        std::vector<cl_device_id> devices(num_devices);
        CL_CHECK_CALL(clGetDeviceIDs(selected_platform, CL_DEVICE_TYPE_GPU, num_devices, devices.data(), nullptr));

        // Select device by name
        cl_device_id selected_device;
        if (num_devices > 1)
        {
            std::size_t selected_device_index{ num_devices };
            std::cout << "Select device by corresponding index\n";
            int device_index{ 0 };
            for (auto device : devices)
            {
                size_t device_name_size;
                CL_CHECK_CALL(clGetDeviceInfo(device, CL_DEVICE_NAME, 0, nullptr, &device_name_size));
                auto device_name{ std::make_unique<char[]>(device_name_size) };
                CL_CHECK_CALL(clGetDeviceInfo(device, CL_DEVICE_NAME, device_name_size, device_name.get(), nullptr));
                std::cout << "[" << device_index++ << "]: " << device_name.get() << "\n";
            }
            do
            {
                std::cout << "Device index: ";
                std::cin >> selected_device_index;
            }
            while (selected_device_index >= num_devices);
            selected_device = devices[selected_device_index];
        }
        else
        {
            selected_device = devices[0];
            size_t device_name_size;
            CL_CHECK_CALL(clGetDeviceInfo(selected_device, CL_DEVICE_NAME, 0, nullptr, &device_name_size));
            auto selected_device_name{ std::make_unique<char[]>(device_name_size) };
            CL_CHECK_CALL(clGetDeviceInfo(selected_device, CL_DEVICE_NAME, device_name_size, selected_device_name.get(),
                                          nullptr));
            std::cout << "Found one device: " << selected_device_name.get() << "\n";
        }

        // Create OpenCL context
        const std::array<cl_context_properties, 3> context_properties{
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(selected_platform),
            0 };
        cl_int err_code{ CL_SUCCESS };
        cl_context context{ clCreateContext(context_properties.data(), 1, &selected_device,
                                            CL::ContextCallback, nullptr, &err_code) };
        CL_CHECK_STATUS(err_code);


        // TODO All up to here should go in a separate class that handles the OpenCL environment
        const CL::Scene scene{ context, scene_description, camera };
        Rendering::CL::RenderingContext rendering_context{ context, selected_device, scene_description, scene };

        const auto start = std::chrono::high_resolution_clock::now();
        rendering_context.Render("render.png");
        const auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Rendering time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";

        // Cleanup
        clReleaseContext(context);

        for (auto device : devices)
        {
            CL_CHECK_CALL(clReleaseDevice(device));
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return 0;
}