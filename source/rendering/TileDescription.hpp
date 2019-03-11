//
// Created by Simon on 2019-03-11.
//

#ifndef RABBIT_TILEDESCRIPTION_HPP
#define RABBIT_TILEDESCRIPTION_HPP

namespace Rendering
{

// Tile description
struct TileDescription
{
    // Tile size
    const unsigned int tile_width, tile_height;
    // Samples per pixel
    const unsigned int pixel_samples;

    constexpr TileDescription(unsigned int tw, unsigned int th, unsigned int ps) noexcept
        : tile_width(tw), tile_height(th), pixel_samples(ps)
    {}

    // Compute the total number of pixels in the tile
    constexpr unsigned int TotalPixels() const noexcept
    {
        return tile_width * tile_height;
    }

    // Compute the total number of samples in the tile
    constexpr unsigned int TotalSamples() const noexcept
    {
        return TotalPixels() * pixel_samples;
    }
};

} // Rendering namespace

#endif //RABBIT_TILEDESCRIPTION_HPP
