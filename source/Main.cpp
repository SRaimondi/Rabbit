#include "RenderingContext.hpp"
#include "TileRenderingContext.hpp"

#include <iostream>

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
        const SceneDescription scene_description{ SceneParser::ReadSceneDescription(argv[1]) };

        // Create camera
        const Rendering::Camera camera{ Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f }, Vector3{ 0.f, 1.f, 0.f },
                                        45.f, scene_description.image_width, scene_description.image_height };

        // Find all platforms
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        if (platforms.empty())
        {
            throw std::runtime_error("No available OpenCL platforms");
        }

        // Select platform by name
        cl::Platform selected_platform;
        if (platforms.size() > 1)
        {
            std::size_t selected_platform_index = platforms.size();
            std::cout << "Select platform by corresponding index\n";
            int platform_index{ 0 };
            for (const cl::Platform& platform : platforms)
            {
                std::cout << "[" << platform_index++ << "]: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";
            }
            do
            {
                std::cout << "Platform index: ";
                std::cin >> selected_platform_index;
            }
            while (selected_platform_index >= platforms.size());
            selected_platform = platforms[selected_platform_index];
        }
        else
        {
            selected_platform = platforms[0];
            std::cout << "Found one platform: " << selected_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
        }

        // Select device from platform
        std::vector<cl::Device> devices;
        selected_platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

        if (devices.empty())
        {
            throw std::runtime_error("No available GPU devices in selected platform");
        }

        // Select device by name
        cl::Device selected_device;
        if (devices.size() > 1)
        {
            std::size_t selected_device_index = devices.size();
            std::cout << "Select device by corresponding index\n";
            int device_index{ 0 };
            for (const cl::Device& device : devices)
            {
                std::cout << "[" << device_index++ << "]: " << device.getInfo<CL_DEVICE_NAME>() << "\n";
            }
            do
            {
                std::cout << "Device index: ";
                std::cin >> selected_device_index;
            }
            while (selected_device_index >= devices.size());
            selected_device = devices[selected_device_index];
        }
        else
        {
            selected_device = devices[0];
            std::cout << "Found one device: " << selected_device.getInfo<CL_DEVICE_NAME>() << "\n";
        }

        // Create RenderingContext
        std::array<cl_context_properties, 3> context_properties{
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(selected_platform()),
            0 };
        Rendering::CL::RenderingContext rendering_context{ selected_device, context_properties.data(),
                                                           scene_description, camera };
        
    }
    catch (const cl::Error& err)
    {
        std::cerr << "OpenCL error: " << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}