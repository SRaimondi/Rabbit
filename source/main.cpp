#include "RenderingContext.hpp"

#include <iostream>

int main()
{
    cl::Platform selected_platform;
    cl::Device selected_device;

    // Select platform and device
    try
    {
        // Find all platforms
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        if (platforms.empty())
        {
            throw std::runtime_error("No available OpenCL platforms");
        }

        // Select platform by name
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
        CL::RenderingContext rendering_context{ selected_device, context_properties.data(), 512, 512 };

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

    // Create the rendering context


    return 0;
}