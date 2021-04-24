#pragma once

#include <filesystem>
#include <vector>

namespace Gris::Graphics
{
struct Mesh;
struct MaterialBlueprint;
}  // namespace Gris::Graphics

namespace Gris::Graphics::Loaders
{

class TinyObjLoaderMeshLoader
{
public:
    TinyObjLoaderMeshLoader() = default;

    static std::tuple<std::vector<Mesh>, std::vector<MaterialBlueprint>> Load(const std::filesystem::path & path);
};

}  // namespace Gris::Graphics::Loaders
