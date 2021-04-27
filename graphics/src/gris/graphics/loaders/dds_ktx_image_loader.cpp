#include <gris/graphics/loaders/dds_ktx_image_loader.h>

#include <gris/engine_exception.h>
#include <gris/graphics/image.h>

#include <gris/utils.h>
#include <gris/assert.h>

#include <dds-ktx.h>

#include <cstring>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Image Gris::Graphics::Loaders::DdsKtxImageLoader::Load(const std::filesystem::path & path)
{
    auto fileContents = Gris::ReadFile<uint8_t>(path);

    auto tc = ddsktx_texture_info{ 0 };
    ddsktx_error err = {};
    if (!ddsktx_parse(&tc, fileContents.data(), static_cast<int>(fileContents.size()), &err))
    {
        throw EngineException("Failed to load DDS texture image " + path.string(), err.msg);
    }

    // TODO: Make this a lot more flexible and so
    GRIS_ALWAYS_ASSERT(tc.format == DDSKTX_FORMAT_BC2, "Format must be DDSKTX_FORMAT_BC2");

    ddsktx_sub_data sub_data;
    ddsktx_get_sub(&tc, &sub_data, fileContents.data(), static_cast<int>(fileContents.size()), 0, 0, 0);

    Image result;
    result.PixelData.resize(sub_data.size_bytes);
    result.Width = static_cast<uint32_t>(sub_data.width);
    result.Height = static_cast<uint32_t>(sub_data.height);
    result.Format = ImageFormat::BC2;

    std::memcpy(result.PixelData.data(), sub_data.buff, sub_data.size_bytes);

    return result;
}
