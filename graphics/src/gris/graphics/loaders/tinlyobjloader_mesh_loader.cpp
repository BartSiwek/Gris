#include <gris/graphics/loaders/tinlyobjloader_mesh_loader.h>

#include <gris/graphics/mesh.h>

#include <gris/engine_exception.h>
#include <gris/utils.h>

#include <tiny_obj_loader.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201)
#endif

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

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

std::vector<Gris::Graphics::Mesh> Gris::Graphics::Loaders::TinyObjLoaderMeshLoader::Load(const std::filesystem::path & path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.string().c_str(), nullptr, false))
    {
        throw Gris::EngineException("Error loading model", err);
    }

    auto uniqueVertices = std::unordered_map<Vertex, uint32_t, VertexHash, VertexComparator>{};

    auto result = Mesh{};
    for (auto const & shape : shapes)
    {
        for (auto const & index : shape.mesh.indices)
        {
            auto vertex = Vertex{};

            vertex.Position = {
                attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 0],
                attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 1],
                attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 2]
            };

            vertex.TextureCoords = {
                attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 0],
                1.0F - attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 1]
            };

            vertex.Color = { 1.0F, 1.0F, 1.0F };

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(result.Vertices.size());
                result.Vertices.push_back(vertex);
            }

            result.Indices.push_back(uniqueVertices[vertex]);
        }
    }

    return { result };
}
