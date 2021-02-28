#include <gris/graphics/loaders/assimp_mesh_loader.h>

#include <gris/graphics/mesh.h>

#include <gris/engine_exception.h>
#include <gris/utils.h>
#include <gris/log.h>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// -------------------------------------------------------------------------------------------------

constexpr static unsigned int DEFAULT_ASSIMP_FLAGS = aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;

// -------------------------------------------------------------------------------------------------

std::vector<Gris::Graphics::Mesh> Gris::Graphics::Loaders::AssimpMeshLoader::Load(const std::filesystem::path & path)
{
    static const aiVector3D ZERO_VECTOR(0.0F, 0.0F, 0.0F);

    aiLogStream grisLoggerStream;
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

    // TODO: auto numVertices = 0;
    auto result = MakeReservedVector<Mesh>(scene->mNumMeshes);
    for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
    {
        // TODO: auto baseVertex = numVertices;
        auto const * currentMesh = scene->mMeshes[meshIndex];

        auto mesh = Mesh{};
        for (size_t vertexIndex = 0; vertexIndex < scene->mMeshes[meshIndex]->mNumVertices; ++vertexIndex)
        {
            auto vertex = Vertex{};

            auto const & position = currentMesh->mVertices[vertexIndex];
            vertex.Position = { position.x, position.y, position.z };

            auto const & texCoord = currentMesh->HasTextureCoords(0) ? currentMesh->mTextureCoords[0][vertexIndex] : ZERO_VECTOR;
            vertex.TextureCoords = { texCoord.x, 1.0F - texCoord.y };

            vertex.Color = { 1.0F, 1.0F, 1.0F };

            mesh.Vertices.emplace_back(std::move(vertex));
        }

		for (size_t faceIndex = 0; faceIndex < currentMesh->mNumFaces; faceIndex++)
        {
            auto const & face = currentMesh->mFaces[faceIndex];
            if (face.mNumIndices != 3)
                continue;

            mesh.Indices.emplace_back(face.mIndices[0]);
            mesh.Indices.emplace_back(face.mIndices[1]);
            mesh.Indices.emplace_back(face.mIndices[2]);
        }

        // TODO: numVertices += scene->mMeshes[meshIndex]->mNumVertices;

        result.emplace_back(std::move(mesh));
    }

    aiReleaseImport(scene);
    aiDetachLogStream(&grisLoggerStream);

    return result;
}
