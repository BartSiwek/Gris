#pragma once

#include <cstdint>
#include <vector>

namespace Gris::Graphics
{

enum class ImageFormat
{
    Undefined,

    // Compressed formats
    BC2,

    // Regular formats
    R8G8B8A8SRGB,

    // Count - must be last
    FormatCount,
};

struct Image
{
    std::vector<uint8_t> PixelData = {};
    uint32_t Width = 0;
    uint32_t Height = 0;
    ImageFormat Format = ImageFormat::Undefined;
};

}  // namespace Gris::Graphics
