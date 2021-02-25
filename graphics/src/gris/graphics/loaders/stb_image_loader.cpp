#include <gris/graphics/loaders/stb_image_loader.h>

#include <gris/engine_exception.h>
#include <gris/graphics/image.h>

#include <stb_image.h>

#include <cstring>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Image Gris::Graphics::Loaders::StbImageLoader::Load(const std::filesystem::path & path)
{
    int texWidth = 0;
    int texHeight = 0;
    int texChannels = 0;
    auto * pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (pixels == nullptr)
    {
        throw Gris::EngineException("Failed to load texture image", path.string());
    }

    auto const forcedChannelCount = 4;
    auto dataSize = static_cast<size_t>(texWidth) * static_cast<size_t>(texHeight) * forcedChannelCount;

    Image result;
    result.PixelData.resize(dataSize);
    result.Width = static_cast<uint32_t>(texWidth);
    result.Height = static_cast<uint32_t>(texHeight);
    result.Channels = forcedChannelCount;

    std::memcpy(result.PixelData.data(), pixels, dataSize);

    stbi_image_free(pixels);

    return result;
}
