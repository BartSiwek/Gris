#pragma once

#include <filesystem>

namespace Gris::Graphics
{
struct Image;
}

namespace Gris::Graphics::Loaders
{

class DdsKtxImageLoader
{
public:
    DdsKtxImageLoader() = default;

    static Image Load(const std::filesystem::path & path);
};

}  // namespace Gris::Graphics::Loaders
