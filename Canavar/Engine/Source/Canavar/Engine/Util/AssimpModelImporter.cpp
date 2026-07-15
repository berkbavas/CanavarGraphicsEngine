#include "AssimpModelImporter.h"

#include "Canavar/Engine/Util/Logger.h"

#include <QDir>

QMap<QString, Canavar::Engine::ScenePtr> Canavar::Engine::AssimpModelImporter::Import(const QString& ScenesDirectory, const QStringList& Formats)
{
    LOG_INFO("AssimpModelImporter::Import: Loading models at {} whose extensions are {}", ScenesDirectory.toStdString(), Formats.join(", ").toStdString());

    QMap<QString, ScenePtr> SceneMap;

    const QDir ScenesDir(ScenesDirectory);
    const QStringList Dirs = ScenesDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    LOG_INFO("AssimpModelImporter::Import: Found {} directories at {}", Dirs.size(), ScenesDirectory.toStdString());

    for (const auto& ChildDirName : Dirs)
    {
        QDir SceneDirPath(ScenesDir.path() + "/" + ChildDirName);
        SceneDirPath.setNameFilters(Formats);
        const QStringList Files = SceneDirPath.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

        for (const auto& File : Files)
        {
            const QString SceneDirPathStr = SceneDirPath.path();
            const QString SceneFilePath = SceneDirPathStr + "/" + File;
            ScenePtr pScene = Import(ChildDirName, SceneDirPathStr, SceneFilePath);

            if (pScene)
            {
                SceneMap[pScene->GetSceneName()] = pScene;
            }
            else
            {
                LOG_WARN("AssimpModelImporter::Import: Scene {} at {} could not be loaded.", ChildDirName.toStdString(), SceneFilePath.toStdString());
            }
        }
    }

    LOG_INFO("AssimpModelImporter::Import: All scenes are loaded at {}", ScenesDirectory.toStdString());

    return SceneMap;
}

Canavar::Engine::ScenePtr Canavar::Engine::AssimpModelImporter::Import(const QString& SceneName, const QString& SceneDirPath, const QString& SceneFilePath)
{
    LOG_DEBUG("AssimpModelImporter::Import: Loading scene: {}", SceneFilePath.toStdString());

    Assimp::Importer Importer;

    const aiScene* aiScene = Importer.ReadFile(SceneFilePath.toStdString(),
                                               aiProcess_Triangulate |          //
                                                   aiProcess_GenSmoothNormals | //
                                                   aiProcess_FlipUVs |          //
                                                   aiProcess_CalcTangentSpace);

    if (aiScene == nullptr)
    {
        LOG_FATAL("AssimpModelImporter::Import: aiScene is nullptr. Error: {}", Importer.GetErrorString());
        return nullptr;
    }

    if (aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        LOG_FATAL("AssimpModelImporter::Import: AI_SCENE_FLAGS_INCOMPLETE is true. Error: {}", Importer.GetErrorString());
        return nullptr;
    }

    if (aiScene->mRootNode == nullptr)
    {
        LOG_FATAL("AssimpModelImporter::Import: scene->mRootNode is nullptr. Error: {}", Importer.GetErrorString());
        return nullptr;
    }

    QVector<TextureMaterialPtr> SceneMaterials;
    SceneMaterials.reserve(aiScene->mNumMaterials);
    QVector<MeshPtr> SceneMeshes;
    SceneMeshes.reserve(aiScene->mNumMeshes);

    // Process materials
    for (unsigned int Index = 0; Index < aiScene->mNumMaterials; ++Index)
    {
        const auto pAiMaterial = aiScene->mMaterials[Index];
        SceneMaterials << ProcessMaterial(aiScene, pAiMaterial, SceneDirPath);
    }

    // Process meshes
    for (unsigned int Index = 0; Index < aiScene->mNumMeshes; ++Index)
    {
        const auto MaterialIndex = aiScene->mMeshes[Index]->mMaterialIndex;
        TextureMaterialPtr pMaterial = SceneMaterials[MaterialIndex];
        const auto pAiMesh = aiScene->mMeshes[Index];
        SceneMeshes << ProcessMesh(pAiMesh, Index, pMaterial);
    }

    const auto pRootNode = ProcessNode(SceneMeshes, aiScene->mRootNode);

    return std::make_shared<Scene>(SceneName, SceneMeshes, SceneMaterials, pRootNode);
}

Canavar::Engine::SceneNodePtr Canavar::Engine::AssimpModelImporter::ProcessNode(const QVector<MeshPtr>& SceneMeshes, aiNode* aiNode)
{
    QVector<MeshWeakPtr> NodeMeshes;
    QVector<SceneNodePtr> Children;

    for (unsigned int MeshIndex = 0; MeshIndex < aiNode->mNumMeshes; ++MeshIndex)
    {
        const auto SceneMeshIndex = aiNode->mMeshes[MeshIndex];
        NodeMeshes << SceneMeshes[SceneMeshIndex];
    }

    for (unsigned int Index = 0; Index < aiNode->mNumChildren; ++Index)
    {
        const auto ChildNode = aiNode->mChildren[Index];
        Children << ProcessNode(SceneMeshes, ChildNode);
    }

    const auto Transformation = QMatrix4x4(aiNode->mTransformation[0]);

    return std::make_shared<SceneNode>(Transformation, Children, NodeMeshes);
}

Canavar::Engine::MeshPtr Canavar::Engine::AssimpModelImporter::ProcessMesh(aiMesh* aiMesh, int MeshId, TextureMaterialPtr pTextureMaterial)
{
    const auto& Vertices = ProcessVertices(aiMesh);
    const auto& TrianglesFaces = ProcessTriangleFaces(aiMesh);
    const auto MeshName = QString::fromUtf8(aiMesh->mName.C_Str());

    return std::make_shared<Canavar::Engine::Mesh>(MeshName, MeshId, Vertices, TrianglesFaces, pTextureMaterial);
}

QVector<Canavar::Engine::Vertex> Canavar::Engine::AssimpModelImporter::ProcessVertices(aiMesh* aiMesh)
{
    QVector<Vertex> Vertices;
    Vertices.reserve(aiMesh->mNumVertices);

    Vertex TempVertex;

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        TempVertex.Position.setX(aiMesh->mVertices[i].x);
        TempVertex.Position.setY(aiMesh->mVertices[i].y);
        TempVertex.Position.setZ(aiMesh->mVertices[i].z);

        if (aiMesh->HasNormals())
        {
            TempVertex.Normal.setX(aiMesh->mNormals[i].x);
            TempVertex.Normal.setY(aiMesh->mNormals[i].y);
            TempVertex.Normal.setZ(aiMesh->mNormals[i].z);
        }

        if (aiMesh->mTextureCoords[0])
        {
            TempVertex.UV.setX(aiMesh->mTextureCoords[0][i].x);
            TempVertex.UV.setY(aiMesh->mTextureCoords[0][i].y);
        }

        if (aiMesh->HasTangentsAndBitangents())
        {
            TempVertex.Tangent.setX(aiMesh->mTangents[i].x);
            TempVertex.Tangent.setY(aiMesh->mTangents[i].y);
            TempVertex.Tangent.setZ(aiMesh->mTangents[i].z);

            TempVertex.Bitangent.setX(aiMesh->mBitangents[i].x);
            TempVertex.Bitangent.setY(aiMesh->mBitangents[i].y);
            TempVertex.Bitangent.setZ(aiMesh->mBitangents[i].z);
        }

        Vertices.push_back(TempVertex);
    }

    return Vertices;
}

QVector<Canavar::Engine::TriangleFace> Canavar::Engine::AssimpModelImporter::ProcessTriangleFaces(aiMesh* aiMesh)
{
    QVector<TriangleFace> TriangleFaces;
    TriangleFaces.reserve(aiMesh->mNumFaces);

    TriangleFace TempFace;

    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace aiFace = aiMesh->mFaces[i];

        if (aiFace.mNumIndices != 3)
        {
            LOG_WARN("AssimpModelImporter::ProcessTriangleFaces: Face {} does not have 3 indices. It has {} indices. Skipping.", i, aiFace.mNumIndices);
            continue;
        }

        TempFace.Idx0 = aiFace.mIndices[0];
        TempFace.Idx1 = aiFace.mIndices[1];
        TempFace.Idx2 = aiFace.mIndices[2];

        TriangleFaces.push_back(TempFace);
    }

    return TriangleFaces;
}

Canavar::Engine::TextureMaterialPtr Canavar::Engine::AssimpModelImporter::ProcessMaterial(const aiScene* pScene, aiMaterial* aiMaterial, const QString& Directory)
{
    const auto Opacity = CalculateMaterialOpacity(aiMaterial);
    const auto ClampedOpacity = std::clamp(Opacity, 0.0f, 1.0f);

    TextureMaterialPtr pMaterial = std::make_shared<TextureMaterial>(ClampedOpacity);

    std::map<const char*, bool> Results;
    Results["aiTextureType_BASE_COLOR"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_BASE_COLOR, TextureType::BaseColor, Directory, 4);
    Results["aiTextureType_METALNESS"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_METALNESS, TextureType::Metallic, Directory, 1);
    Results["aiTextureType_DIFFUSE_ROUGHNESS"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_DIFFUSE_ROUGHNESS, TextureType::Roughness, Directory, 1);
    Results["aiTextureType_AMBIENT_OCCLUSION"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_AMBIENT_OCCLUSION, TextureType::AmbientOcclusion, Directory, 1);
    Results["aiTextureType_NORMALS"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_NORMALS, TextureType::Normal, Directory, 4);

    for (const auto& [Key, Value] : Results)
    {
        if (Value)
        {
            LOG_DEBUG("AssimpModelImporter::ProcessMaterial: Texture type {} found for material.", Key);
        }
        else
        {
            LOG_DEBUG("AssimpModelImporter::ProcessMaterial: Texture type {} not found for material.", Key);
        }
    }

    return pMaterial;
}

bool Canavar::Engine::AssimpModelImporter::ProcessTexture(const aiScene* pScene, TextureMaterialPtr pMaterial, aiMaterial* aiMaterial, aiTextureType aiType, TextureType TextureType, const QString& Directory, int ComponentCount)
{
    for (int i = 0; i < aiMaterial->GetTextureCount(aiType); i++)
    {
        aiString Str;
        aiMaterial->GetTexture(aiType, i, &Str);

        if (const auto* pTexture = pScene->GetEmbeddedTexture(Str.C_Str()))
        {
            QImage Image(reinterpret_cast<uchar*>(pTexture->pcData), pTexture->mWidth, pTexture->mHeight, QImage::Format_RGBA8888);

            if (Image.isNull())
            {
                LOG_WARN("AssimpModelImporter::ProcessTexture: Image at '{}' is null.", Str.C_Str());
            }
            else
            {
                LOG_DEBUG("AssimpModelImporter::ProcessTexture: Loading texture for {}", Str.C_Str());
                return pMaterial->LoadTexture(TextureType, Image, ComponentCount);
            }
        }
        else
        {
            QString TextureFilename(Str.C_Str());

            const auto TexturePath = Directory + "/" + TextureFilename;

            QImage Image(TexturePath);

            if (Image.isNull())
            {
                LOG_WARN("AssimpModelImporter::ProcessTexture: Image at '{}' is null.", TexturePath.toStdString());
            }
            else
            {
                LOG_DEBUG("AssimpModelImporter::ProcessTexture: Loading texture for {}", TexturePath.toStdString());
                return pMaterial->LoadTexture(TextureType, Image, ComponentCount);
            }
        }
    }

    return false;
}

float Canavar::Engine::AssimpModelImporter::CalculateMaterialOpacity(aiMaterial* aiMaterial)
{
    float Opacity = 1.0f;

    if (AI_SUCCESS == aiMaterial->Get(AI_MATKEY_OPACITY, Opacity))
    {
        LOG_DEBUG("AssimpModelImporter::CalculateMaterialOpacity: AI_MATKEY_OPACITY found, returning opacity value: {}", Opacity);
    }
    else
    {
        // If opacity not found, try transparency factor
        LOG_DEBUG("AssimpModelImporter::CalculateMaterialOpacity: AI_MATKEY_OPACITY not found, trying AI_MATKEY_TRANSPARENCYFACTOR.");

        if (AI_SUCCESS == aiMaterial->Get(AI_MATKEY_TRANSPARENCYFACTOR, Opacity))
        {
            LOG_DEBUG("AssimpModelImporter::CalculateMaterialOpacity: AI_MATKEY_TRANSPARENCYFACTOR found, returning opacity value: {}", Opacity);
        }
        else
        {
            LOG_DEBUG("AssimpModelImporter::CalculateMaterialOpacity: AI_MATKEY_TRANSPARENCYFACTOR not found as well.");

            aiColor4D Color;

            if (AI_SUCCESS == aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, Color))
            {
                LOG_DEBUG("AssimpModelImporter::CalculateMaterialOpacity: AI_MATKEY_COLOR_DIFFUSE found, returning opacity value: {}", Color.a);
                Opacity = std::clamp(Color.a, 0.0f, 1.0f);
            }
            else
            {
                LOG_DEBUG("AssimpModelImporter::CalculateMaterialOpacity: AI_MATKEY_COLOR_DIFFUSE not found as well, returning default opacity value: 1.0f.");
            }
        }
    }

    return Opacity;
}
