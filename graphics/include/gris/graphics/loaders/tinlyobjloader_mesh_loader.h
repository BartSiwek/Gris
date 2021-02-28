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

    static std::vector<Mesh> Load(const std::filesystem::path & path);
};

}  // namespace Gris::Graphics::Loaders
