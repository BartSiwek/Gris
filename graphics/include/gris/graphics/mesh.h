#pragma once

#include <glm/glm.hpp>

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
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
};

}  // namespace Gris::Graphics
