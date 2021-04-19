#include <gris/graphics/loaders/assimp_mesh_loader.h>

#include <gris/graphics/mesh.h>

#include <gris/engine_exception.h>
#include <gris/log.h>
#include <gris/span.h>
#include <gris/utils.h>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// -------------------------------------------------------------------------------------------------

constexpr static unsigned int DEFAULT_ASSIMP_FLAGS = static_cast<unsigned int>(aiProcess_Triangulate)
                                                     | static_cast<unsigned int>(aiProcess_PreTransformVertices)
                                                     | static_cast<unsigned int>(aiProcess_CalcTangentSpace)
                                                     | static_cast<unsigned int>(aiProcess_GenSmoothNormals)
                                                     | static_cast<unsigned int>(aiProcess_JoinIdenticalVertices)
                                                     | static_cast<unsigned int>(aiProcess_FlipWindingOrder);

// -------------------------------------------------------------------------------------------------

std::vector<Gris::Graphics::Mesh> Gris::Graphics::Loaders::AssimpMeshLoader::Load(const std::filesystem::path & path)
{
    static const aiVector3D ZERO_VECTOR(0.0F, 0.0F, 0.0F);

    aiLogStream grisLoggerStream = {};
    grisLoggerStream.callback = [](const char * message, char * /* user */)
    {
        auto stringMessage = std::string{ message };
        if (stringMessage.back() == '\n')
        {
            stringMessage = stringMessage.substr(0, stringMessage.size() - 1);
        }
        Gris::Log::Debug("[AssimpMeshLoader] {}", stringMessage);
    };

    aiAttachLogStream(&grisLoggerStream);

    auto const * scene = aiImportFile(path.string().c_str(), DEFAULT_ASSIMP_FLAGS);

    if (scene == nullptr)
    {
        throw Gris::EngineException("Error loading model", aiGetErrorString());
    }

    auto result = MakeReservedVector<Mesh>(scene->mNumMeshes);

    auto meshes = Gris::Span<aiMesh *>(scene->mMeshes, scene->mNumMeshes);
    for (const auto & currentMesh : meshes)
    {
        auto mesh = Mesh{};

        auto vertices = Gris::Span<aiVector3D>(currentMesh->mVertices, currentMesh->mNumVertices);
        auto textureCoords0 = Gris::Span<aiVector3D>(currentMesh->mTextureCoords[0], currentMesh->mNumVertices);
        for (size_t vertexIndex = 0; vertexIndex < currentMesh->mNumVertices; ++vertexIndex)
        {
            auto vertex = Vertex{};

            auto const & position = vertices[vertexIndex];
            vertex.Position = { position.x, position.y, position.z };

            auto const & texCoordinates = currentMesh->HasTextureCoords(0) ? textureCoords0[vertexIndex] : ZERO_VECTOR;
            vertex.TextureCoords = { texCoordinates.x, 1.0F - texCoordinates.y };

            vertex.Color = { 1.0F, 1.0F, 1.0F };

            mesh.Vertices.emplace_back(vertex);
        }

        auto faces = Gris::Span<aiFace>(currentMesh->mFaces, currentMesh->mNumFaces);
        for (const auto & face : faces)
        {
            auto indices = Gris::Span<unsigned int>(face.mIndices, face.mNumIndices);

            if (indices.size() != 3)
            {
                continue;
            }

            mesh.Indices.emplace_back(indices[0]);
            mesh.Indices.emplace_back(indices[1]);
            mesh.Indices.emplace_back(indices[2]);
        }

        result.emplace_back(std::move(mesh));
    }

    aiReleaseImport(scene);
    aiDetachLogStream(&grisLoggerStream);

    return result;
}
