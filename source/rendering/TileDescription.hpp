//
// Created by Simon on 2019-03-11.
//

#ifndef RABBIT_TILEDESCRIPTION_HPP
#define RABBIT_TILEDESCRIPTION_HPP

namespace Rendering
{

// Tile description
class TileDescription
{
public:
    constexpr TileDescription(cl_uint tw, cl_uint th, cl_uint ps) noexcept
        : tile_width{ tw }, tile_height{ th }, pixel_samples{ ps },
        total_pixels{ tile_height * tile_width }, total_samples{ total_pixels * pixel_samples }
    {}

    constexpr cl_uint Width() const noexcept
    {
        return tile_width;
    }

    constexpr cl_uint Height() const noexcept
    {
        return tile_height;
    }

    constexpr cl_uint PixelSamples() const noexcept
    {
        return pixel_samples;
    }

    // Compute the total number of pixels in the tile
    constexpr cl_uint TotalPixels() const noexcept
    {
        return total_pixels;
    }

    // Compute the total number of samples in the tile
    constexpr cl_uint TotalSamples() const noexcept
    {
        return total_samples;
    }

private:
    // Tile size
    const cl_uint tile_width, tile_height;
    // Samples per pixel
    const cl_uint pixel_samples;
    // Total number of pixels
    const cl_uint total_pixels;
    // Total number of samples
    const cl_uint total_samples;
};

} // Rendering namespace

#endif //RABBIT_TILEDESCRIPTION_HPP
