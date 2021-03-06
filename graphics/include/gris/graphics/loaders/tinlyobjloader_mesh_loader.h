#pragma once

#include <filesystem>

namespace Gris::Graphics
{
struct Mesh;
}

namespace Gris::Graphics::Loaders
{

class TinyObjLoaderMeshLoader
{
public:
    TinyObjLoaderMeshLoader() = default;

    static Mesh Load(const std::filesystem::path & path);
};

}  // namespace Gris::Graphics::Loaders
