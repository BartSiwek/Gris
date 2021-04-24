#include <gris/graphics/loaders/tinlyobjloader_mesh_loader.h>

#include <gris/graphics/mesh.h>

#include <gris/engine_exception.h>
#include <gris/utils.h>

#include <tiny_obj_loader.h>

#include <glm/gtx/hash.hpp>

#include <unordered_map>

// -------------------------------------------------------------------------------------------------

struct VertexHash
{
    size_t operator()(Gris::Graphics::Vertex const & vertex) const noexcept
    {
        return ((std::hash<glm::vec3>()(vertex.Position) ^ (std::hash<glm::vec3>()(vertex.Color) << 1U)) >> 1U) ^ (std::hash<glm::vec2>()(vertex.TextureCoords) << 1U);
    }
};

// -------------------------------------------------------------------------------------------------

struct VertexComparator
{
    bool operator()(const Gris::Graphics::Vertex & lhs, const Gris::Graphics::Vertex & rhs) const
    {
        return lhs.Position == rhs.Position && lhs.Color == rhs.Color && lhs.TextureCoords == rhs.TextureCoords;
    }
};

// -------------------------------------------------------------------------------------------------

std::tuple<std::vector<Gris::Graphics::Mesh>, std::vector<Gris::Graphics::MaterialBlueprint>> Gris::Graphics::Loaders::TinyObjLoaderMeshLoader::Load(const std::filesystem::path & path)
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!LoadObj(&attributes, &shapes, &materials, &err, path.string().c_str(), nullptr, false))
    {
        throw EngineException("Error loading model", err);
    }

    auto resultMeshes = MakeReservedVector<Mesh>(shapes.size());

    for (auto const & shape : shapes)
    {
        auto uniqueVertices = std::unordered_map<Vertex, uint32_t, VertexHash, VertexComparator>{};
        auto currentMesh = Mesh{};

        for (auto const & index : shape.mesh.indices)
        {
            auto vertex = Vertex{};

            vertex.Position = {
                attributes.vertices[3 * static_cast<size_t>(index.vertex_index) + 0],
                attributes.vertices[3 * static_cast<size_t>(index.vertex_index) + 1],
                attributes.vertices[3 * static_cast<size_t>(index.vertex_index) + 2]
            };

            vertex.TextureCoords = {
                attributes.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 0],
                1.0F - attributes.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 1]
            };

            vertex.Color = { 1.0F, 1.0F, 1.0F };

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(currentMesh.Vertices.size());
                currentMesh.Vertices.push_back(vertex);
            }

            currentMesh.Indices.push_back(uniqueVertices[vertex]);
        }

        resultMeshes.emplace_back(std::move(currentMesh));
    }

    return { resultMeshes, {} };
}
