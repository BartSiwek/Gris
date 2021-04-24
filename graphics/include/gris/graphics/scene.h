#pragma once

#include <gris/graphics/image.h>

#include <glm/glm.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace Gris::Graphics
{

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 TextureCoords;
};

struct Mesh
{
    std::vector<Vertex> Vertices = {};
    std::vector<uint32_t> Indices = {};
    size_t MaterialIndex;
};

struct MaterialBlueprint
{
    std::vector<std::filesystem::path> DiffuseTextures;
    std::vector<std::filesystem::path> SpecularTextures;
    std::vector<std::filesystem::path> NormalTextures;
    std::string Name;
};

struct Material
{
    std::vector<Image> DiffuseTextures;
    std::vector<Image> SpecularTextures;
    std::vector<Image> NormalTextures;
    std::string Name;
};

struct Scene
{
    std::vector<Mesh> Meshes;
    std::vector<Material> Materials;
};

}  // namespace Gris::Graphics
