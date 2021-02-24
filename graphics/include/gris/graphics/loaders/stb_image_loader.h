#pragma once

#include <filesystem>

namespace Gris::Graphics
{
struct Image;
}

namespace Gris::Graphics::Loaders
{

class StbImageLoader
{
public:
    StbImageLoader() = default;

    Image Load(const std::filesystem::path & path);
};

}  // namespace Gris::Graphics
