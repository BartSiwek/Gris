#pragma once

#include <filesystem>
#include <vector>

namespace Gris::Graphics
{
struct Mesh;
}

namespace Gris::Graphics::Loaders
{

class AssimpMeshLoader
{
public:
    AssimpMeshLoader() = default;

    static std::vector<Gris::Graphics::Mesh> Load(const std::filesystem::path & path);
};

}  // namespace Gris::Graphics::Loaders
