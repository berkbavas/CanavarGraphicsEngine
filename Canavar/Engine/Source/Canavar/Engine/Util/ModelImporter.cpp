#include "ModelImporter.h"

#include "Canavar/Engine/Core/AABB.h"
#include "Canavar/Engine/Util/Logger.h"

#include <QDir>

std::map<std::string, Canavar::Engine::ScenePtr> Canavar::Engine::ModelImporter::Import(const QString& Directory, const QStringList& Formats)
{
    qInfo() << "Loading and models at" << Directory << "whose extensions are" << Formats;

    std::map<std::string, ScenePtr> Result;

    QDir Dir(Directory);
    QStringList Dirs = Dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    qInfo() << "All directories are: " << Dirs;

    for (const auto& DirName : Dirs)
    {
        QDir ChildDir(Dir.path() + "/" + DirName);
        ChildDir.setNameFilters(Formats);
        QStringList Files = ChildDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

        for (const auto& File : Files)
        {
            QString Path = ChildDir.path() + "/" + File;
            ScenePtr pScene = Import(DirName, Path);

            if (pScene)
            {
                Result.insert(std::make_pair(pScene->GetSceneName(), pScene));
            }
            else
            {
                qWarning() << "Model" << DirName << "at" << Path << "could not be loaded.";
            }
        }
    }

    qInfo() << "All models are loaded at" << Directory;

    return Result;
}

Canavar::Engine::ScenePtr Canavar::Engine::ModelImporter::Import(const QString& SceneName, const QString& Fullpath)
{
    LOG_DEBUG("Importer::Import: Loading model: {}", Fullpath.toStdString());

    Assimp::Importer Importer;

    const aiScene* aiScene = Importer.ReadFile(Fullpath.toStdString(),
                                               aiProcess_Triangulate |          //
                                                   aiProcess_GenSmoothNormals | //
                                                   aiProcess_FlipUVs |          //
                                                   aiProcess_CalcTangentSpace | //
                                                   aiProcess_GenBoundingBoxes);
    if (aiScene == nullptr)
    {
        LOG_FATAL("Importer::Import: scene is nullptr. Error: {}", Importer.GetErrorString());
        return nullptr;
    }

    if (aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        LOG_FATAL("Importer::Import: AI_SCENE_FLAGS_INCOMPLETE is true. Error: {}", Importer.GetErrorString());
        return nullptr;
    }

    if (aiScene->mRootNode == nullptr)
    {
        LOG_FATAL("Importer::Import: scene->mRootNode is nullptr. Error: {}", Importer.GetErrorString());
        return nullptr;
    }

    const int LastIndexOf = Fullpath.lastIndexOf("/");
    const QString Directory = Fullpath.left(LastIndexOf);
    const QString Filename = Fullpath.right(LastIndexOf);

    ScenePtr pScene = std::make_shared<Scene>();
    pScene->SetSceneName(SceneName.toStdString());

    // Materials
    for (unsigned int i = 0; i < aiScene->mNumMaterials; ++i)
    {
        MaterialPtr pMaterial = ProcessMaterial(aiScene, aiScene->mMaterials[i], Directory);
        pScene->AddMaterial(pMaterial);
    }

    // Meshes
    for (unsigned int i = 0; i < aiScene->mNumMeshes; i++)
    {
        MaterialPtr pMaterial = pScene->GetMaterial(aiScene->mMeshes[i]->mMaterialIndex);
        MeshPtr pMesh = ProcessMesh(aiScene->mMeshes[i]);
        pMesh->SetMeshId(i);
        pMesh->SetMaterial(pMaterial);
        pMesh->SetOpacity(pMaterial->GetOpacity());

        pScene->AddMesh(pMesh);
    }

    qDebug() << "ModelImporter::Import: Number of meshes:" << aiScene->mNumMeshes;

    // Nodes
    SceneNodePtr pRootNode = ProcessNode(pScene, aiScene->mRootNode);
    pScene->SetRootNode(pRootNode);
    CalculateAABB(pScene);

    return pScene;
}

Canavar::Engine::SceneNodePtr Canavar::Engine::ModelImporter::ProcessNode(ScenePtr pScene, aiNode* aiNode)
{
    SceneNodePtr pNode = std::make_shared<SceneNode>();

    for (unsigned int i = 0; i < aiNode->mNumMeshes; ++i)
    {
        MeshPtr pMesh = pScene->GetMesh(aiNode->mMeshes[i]);
        pNode->AddMesh(pMesh);
    }

    for (unsigned int i = 0; i < aiNode->mNumChildren; ++i)
    {
        SceneNodePtr pChildNode = ProcessNode(pScene, aiNode->mChildren[i]);
        pNode->AddChild(pChildNode);
    }

    pNode->SetNodeName(aiNode->mName.C_Str());
    pNode->SetTransformation(ToQMatrix4x4(aiNode->mTransformation));

    return pNode;
}

Canavar::Engine::MeshPtr Canavar::Engine::ModelImporter::ProcessMesh(aiMesh* aiMesh)
{
    MeshPtr pMesh = std::make_shared<Mesh>();

    Vertex Vertex;

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex.Position.setX(aiMesh->mVertices[i].x);
        Vertex.Position.setY(aiMesh->mVertices[i].y);
        Vertex.Position.setZ(aiMesh->mVertices[i].z);

        if (aiMesh->HasNormals())
        {
            Vertex.Normal.setX(aiMesh->mNormals[i].x);
            Vertex.Normal.setY(aiMesh->mNormals[i].y);
            Vertex.Normal.setZ(aiMesh->mNormals[i].z);
        }

        if (aiMesh->mTextureCoords[0])
        {
            Vertex.Texture.setX(aiMesh->mTextureCoords[0][i].x);
            Vertex.Texture.setY(aiMesh->mTextureCoords[0][i].y);
        }

        if (aiMesh->HasTangentsAndBitangents())
        {
            Vertex.Tangent.setX(aiMesh->mTangents[i].x);
            Vertex.Tangent.setY(aiMesh->mTangents[i].y);
            Vertex.Tangent.setZ(aiMesh->mTangents[i].z);

            Vertex.Bitangent.setX(aiMesh->mBitangents[i].x);
            Vertex.Bitangent.setY(aiMesh->mBitangents[i].y);
            Vertex.Bitangent.setZ(aiMesh->mBitangents[i].z);
        }

        pMesh->AddVertex(Vertex);
    }

    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace aiFace = aiMesh->mFaces[i];

        for (unsigned int j = 0; j < aiFace.mNumIndices; j++)
        {
            pMesh->AddIndex(aiFace.mIndices[j]);
        }
    }

    pMesh->SetMeshName(aiMesh->mName.C_Str());

    const auto Min = aiMesh->mAABB.mMin;
    const auto Max = aiMesh->mAABB.mMax;
    pMesh->SetAABB(AABB{ QVector3D(Min.x, Min.y, Min.z), QVector3D(Max.x, Max.y, Max.z) });

    return pMesh;
}

Canavar::Engine::MaterialPtr Canavar::Engine::ModelImporter::ProcessMaterial(const aiScene* pScene, aiMaterial* aiMaterial, const QString& Directory)
{
    MaterialPtr pMaterial = std::make_shared<Material>();

    std::map<const char*, bool> Results;
    Results["aiTextureType_BASE_COLOR"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_BASE_COLOR, TextureType::BaseColor, Directory, 4);
    Results["aiTextureType_METALNESS"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_METALNESS, TextureType::Metallic, Directory, 1);
    Results["aiTextureType_DIFFUSE_ROUGHNESS"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_DIFFUSE_ROUGHNESS, TextureType::Roughness, Directory, 1);
    Results["aiTextureType_AMBIENT_OCCLUSION"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_AMBIENT_OCCLUSION, TextureType::AmbientOcclusion, Directory, 1);
    Results["aiTextureType_NORMALS"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_NORMALS, TextureType::Normal, Directory, 4);

    for (const auto [TextureType, Result] : Results)
    {
        qDebug() << "ModelImporter::ProcessMaterial:" << TextureType << ":" << Result;
    }

    // Process opacity
    float Opacity = 1.0f;
    if (AI_SUCCESS == aiMaterial->Get(AI_MATKEY_OPACITY, Opacity))
    {
        LOG_DEBUG("ModelImporter::ProcessMaterial: AI_MATKEY_OPACITY found, setting opacity to {}.", Opacity);
        pMaterial->SetOpacity(Opacity);
    }
    else
    {
        // If opacity not found, try transparency factor
        LOG_DEBUG("ModelImporter::ProcessMaterial: AI_MATKEY_OPACITY not found, trying AI_MATKEY_TRANSPARENCYFACTOR.");

        if (AI_SUCCESS == aiMaterial->Get(AI_MATKEY_TRANSPARENCYFACTOR, Opacity))
        {
            LOG_DEBUG("ModelImporter::ProcessMaterial: AI_MATKEY_TRANSPARENCYFACTOR found, setting opacity to {}.", Opacity);
            pMaterial->SetOpacity(Opacity);
        }
        else
        {
            LOG_DEBUG("ModelImporter::ProcessMaterial: AI_MATKEY_TRANSPARENCYFACTOR not found as well.");

            aiColor4D Color;

            if (AI_SUCCESS == aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, Color))
            {
                LOG_DEBUG("ModelImporter::ProcessMaterial: AI_MATKEY_COLOR_DIFFUSE found, setting opacity to {}.", Color.a);
                pMaterial->SetOpacity(Color.a);
            }
            else
            {
                LOG_DEBUG("ModelImporter::ProcessMaterial: AI_MATKEY_COLOR_DIFFUSE not found as well, setting default opacity to 1.0f.");
                pMaterial->SetOpacity(1.0f);
            }
        }
    }

    return pMaterial;
}

bool Canavar::Engine::ModelImporter::ProcessTexture(const aiScene* pScene, MaterialPtr pMaterial, aiMaterial* aiMaterial, aiTextureType aiType, TextureType Type, const QString& Directory, int components)
{
    for (int i = 0; i < aiMaterial->GetTextureCount(aiType); i++)
    {
        aiString Str;
        aiMaterial->GetTexture(aiType, i, &Str);

        if (const auto* pTexture = pScene->GetEmbeddedTexture(Str.C_Str()))
        {
            LOG_DEBUG("ModelImporter::ProcessTexture: Loading embedded texture for {}", Str.C_Str());
            QImage Image(reinterpret_cast<uchar*>(pTexture->pcData), pTexture->mWidth, pTexture->mHeight, QImage::Format_RGBA8888);
            return pMaterial->LoadTextureFromImage(Type, Image, components);
        }
        else
        {
            QString Filename(Str.C_Str());
            QString Path = Directory + "/" + Filename;

            LOG_DEBUG("ModelImporter::ProcessTexture: Loading texture for {}", Path.toStdString());
            return pMaterial->LoadTextureFromPath(Type, Path, components);
        }
    }

    return false;
}

void Canavar::Engine::ModelImporter::CalculateAABB(ScenePtr pScene)
{
    const auto pRootNode = pScene->GetRootNode();
    const auto AABB = pRootNode->CalculateAABB(pRootNode->GetTransformation());

    pScene->SetAABB(AABB);
}

QMatrix4x4 Canavar::Engine::ModelImporter::ToQMatrix4x4(const aiMatrix4x4& aiMatrix)
{
    return QMatrix4x4(aiMatrix[0]);
}

QOpenGLTexture* Canavar::Engine::ModelImporter::CreateTexture(const QString& Path)
{
    QImage Image(Path);

    if (Image.isNull())
    {
        CGE_EXIT_FAILURE("ModelImporter::CreateTexture: Image at '{}' is null.", Path.toStdString());
    }

    QOpenGLTexture* pTexture = new QOpenGLTexture(Image, QOpenGLTexture::GenerateMipMaps);
    pTexture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    pTexture->setMinMagFilters(QOpenGLTexture::Filter::LinearMipMapLinear, QOpenGLTexture::Filter::Linear);

    return pTexture;
}
