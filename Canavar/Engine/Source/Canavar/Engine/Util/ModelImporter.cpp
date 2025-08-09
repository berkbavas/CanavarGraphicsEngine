#include "ModelImporter.h"

#include "Canavar/Engine/Core/AABB.h"
#include "Canavar/Engine/Util/Logger.h"

#include <QDir>

std::map<std::string, Canavar::Engine::ScenePtr> Canavar::Engine::ModelImporter::Import(const QString& directory, const QStringList& formats)
{
    qInfo() << "Loading and models at" << directory << "whose extensions are" << formats;

    std::map<std::string, ScenePtr> result;

    QDir dir(directory);
    QStringList dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    qInfo() << "All directories are: " << dirs;

    for (const auto& dirName : dirs)
    {
        QDir childDir(dir.path() + "/" + dirName);
        childDir.setNameFilters(formats);
        QStringList files = childDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

        for (const auto& file : files)
        {
            QString path = childDir.path() + "/" + file;
            ScenePtr pScene = Import(dirName, path);

            if (pScene)
            {
                result.insert(std::make_pair(pScene->GetSceneName(), pScene));
            }
            else
            {
                qWarning() << "Model" << dirName << "at" << path << "could not be loaded.";
            }
        }
    }

    qInfo() << "All models are loaded at" << directory;

    return result;
}

Canavar::Engine::ScenePtr Canavar::Engine::ModelImporter::Import(const QString& sceneName, const QString& fullpath)
{
    LOG_DEBUG("Importer::Import: Loading model: {}", fullpath.toStdString());

    Assimp::Importer importer;

    const aiScene* aiScene = importer.ReadFile(fullpath.toStdString(),
                                               aiProcess_Triangulate |          //
                                                   aiProcess_GenSmoothNormals | //
                                                   aiProcess_FlipUVs |          //
                                                   aiProcess_CalcTangentSpace | //
                                                   aiProcess_GenBoundingBoxes);
    if (aiScene == nullptr)
    {
        LOG_FATAL("Importer::Import: scene is nullptr. Error: {}", importer.GetErrorString());
        return nullptr;
    }

    if (aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        LOG_FATAL("Importer::Import: AI_SCENE_FLAGS_INCOMPLETE is true. Error: {}", importer.GetErrorString());
        return nullptr;
    }

    if (aiScene->mRootNode == nullptr)
    {
        LOG_FATAL("Importer::Import: scene->mRootNode is nullptr. Error: {}", importer.GetErrorString());
        return nullptr;
    }

    const int lastIndexOf = fullpath.lastIndexOf("/");
    const QString directory = fullpath.left(lastIndexOf);
    const QString filename = fullpath.right(lastIndexOf);

    ScenePtr pScene = std::make_shared<Scene>();
    pScene->SetSceneName(sceneName.toStdString());

    // Materials
    for (unsigned int i = 0; i < aiScene->mNumMaterials; ++i)
    {
        MaterialPtr pMaterial = ProcessMaterial(aiScene, aiScene->mMaterials[i], directory);
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

    Vertex vertex;

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        vertex.position.setX(aiMesh->mVertices[i].x);
        vertex.position.setY(aiMesh->mVertices[i].y);
        vertex.position.setZ(aiMesh->mVertices[i].z);

        if (aiMesh->HasNormals())
        {
            vertex.normal.setX(aiMesh->mNormals[i].x);
            vertex.normal.setY(aiMesh->mNormals[i].y);
            vertex.normal.setZ(aiMesh->mNormals[i].z);
        }

        if (aiMesh->mTextureCoords[0])
        {
            vertex.texture.setX(aiMesh->mTextureCoords[0][i].x);
            vertex.texture.setY(aiMesh->mTextureCoords[0][i].y);
        }

        if (aiMesh->HasTangentsAndBitangents())
        {
            vertex.tangent.setX(aiMesh->mTangents[i].x);
            vertex.tangent.setY(aiMesh->mTangents[i].y);
            vertex.tangent.setZ(aiMesh->mTangents[i].z);

            vertex.bitangent.setX(aiMesh->mBitangents[i].x);
            vertex.bitangent.setY(aiMesh->mBitangents[i].y);
            vertex.bitangent.setZ(aiMesh->mBitangents[i].z);
        }

        pMesh->AddVertex(vertex);
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

    const auto min = aiMesh->mAABB.mMin;
    const auto max = aiMesh->mAABB.mMax;
    pMesh->SetAABB(AABB{ QVector3D(min.x, min.y, min.z), QVector3D(max.x, max.y, max.z) });

    return pMesh;
}

Canavar::Engine::MaterialPtr Canavar::Engine::ModelImporter::ProcessMaterial(const aiScene* pScene, aiMaterial* aiMaterial, const QString& directory)
{
    MaterialPtr pMaterial = std::make_shared<Material>();

    std::map<const char*, bool> results;
    results["aiTextureType_BASE_COLOR"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_BASE_COLOR, TextureType::BaseColor, directory, 4);
    results["aiTextureType_METALNESS"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_METALNESS, TextureType::Metallic, directory, 1);
    results["aiTextureType_DIFFUSE_ROUGHNESS"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_DIFFUSE_ROUGHNESS, TextureType::Roughness, directory, 1);
    results["aiTextureType_AMBIENT_OCCLUSION"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_AMBIENT_OCCLUSION, TextureType::AmbientOcclusion, directory, 1);
    results["aiTextureType_NORMALS"] = ProcessTexture(pScene, pMaterial, aiMaterial, aiTextureType_NORMALS, TextureType::Normal, directory, 4);

    for (const auto [textureType, result] : results)
    {
        qDebug() << "ModelImporter::ProcessMaterial:" << textureType << ":" << result;
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

bool Canavar::Engine::ModelImporter::ProcessTexture(const aiScene* pScene, MaterialPtr pMaterial, aiMaterial* aiMaterial, aiTextureType aiType, TextureType type, const QString& directory, int components)
{
    for (int i = 0; i < aiMaterial->GetTextureCount(aiType); i++)
    {
        aiString str;
        aiMaterial->GetTexture(aiType, i, &str);

        if (const auto* pTexture = pScene->GetEmbeddedTexture(str.C_Str()))
        {
            LOG_DEBUG("ModelImporter::ProcessTexture: Loading embedded texture for {}", str.C_Str());
            QImage image(reinterpret_cast<uchar*>(pTexture->pcData), pTexture->mWidth, pTexture->mHeight, QImage::Format_RGBA8888);
            return pMaterial->LoadTextureFromImage(type, image, components);
        }
        else
        {
            QString filename(str.C_Str());
            QString path = directory + "/" + filename;

            LOG_DEBUG("ModelImporter::ProcessTexture: Loading texture for {}", path.toStdString());
            return pMaterial->LoadTextureFromPath(type, path, components);
        }
    }

    return false;
}

void Canavar::Engine::ModelImporter::CalculateAABB(ScenePtr pScene)
{
    const auto pRootNode = pScene->GetRootNode();
    const auto AABB = pRootNode->CalculateAABB();

    pScene->SetAABB(AABB);
}

QMatrix4x4 Canavar::Engine::ModelImporter::ToQMatrix4x4(const aiMatrix4x4& aiMatrix)
{
    return QMatrix4x4(aiMatrix[0]);
}

QOpenGLTexture* Canavar::Engine::ModelImporter::CreateTexture(const QString& path)
{
    QImage image(path);

    if (image.isNull())
    {
        CGE_EXIT_FAILURE("ModelImporter::CreateTexture: Image at '{}' is null.", path.toStdString());
    }

    QOpenGLTexture* pTexture = new QOpenGLTexture(image, QOpenGLTexture::GenerateMipMaps);
    pTexture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    pTexture->setMinMagFilters(QOpenGLTexture::Filter::LinearMipMapLinear, QOpenGLTexture::Filter::Linear);

    return pTexture;
}
