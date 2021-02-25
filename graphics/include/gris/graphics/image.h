#pragma once

#include <cstdint>
#include <vector>

namespace Gris::Graphics
{

struct Image
{
    std::vector<uint8_t> PixelData = {};
    uint32_t Width = 0;
    uint32_t Height = 0;
    uint32_t Channels = 0;
};

}  // namespace Gris::Graphics
