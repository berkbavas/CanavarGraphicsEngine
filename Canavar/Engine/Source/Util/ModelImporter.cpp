#include "ModelImporter.h"

#include "Util/Logger.h"

#include <QDir>

std::map<QString, Canavar::Engine::ScenePtr> Canavar::Engine::ModelImporter::Import(const QString& directory, const QStringList& formats)
{
    qInfo() << "Loading and models at" << directory << "whose extensions are" << formats;

    std::map<QString, ScenePtr> result;

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

    const aiScene* aiScene = importer.ReadFile(fullpath.toStdString(),          //
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
    pScene->SetSceneName(sceneName);

    // Materials
    for (unsigned int i = 0; i < aiScene->mNumMaterials; ++i)
    {
        MaterialPtr pMaterial = ProcessMaterial(aiScene->mMaterials[i], directory);
        pScene->AddMaterial(pMaterial);
    }

    // Meshes
    for (unsigned int i = 0; i < aiScene->mNumMeshes; i++)
    {
        MaterialPtr pMaterial = pScene->GetMaterial(aiScene->mMeshes[i]->mMaterialIndex);
        MeshPtr pMesh = ProcessMesh(aiScene->mMeshes[i]);
        pMesh->SetMaterial(pMaterial);
        pScene->AddMesh(pMesh);
    }

    // Nodes
    SceneNodePtr pRootNode = ProcessNode(pScene, aiScene->mRootNode);
    pScene->SetRootNode(pRootNode);

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

    pNode->SetNodeName(aiNode->mName.C_Str());

    // TODO: Set world transformation here

    for (unsigned int i = 0; i < aiNode->mNumChildren; ++i)
    {
        SceneNodePtr pChildNode = ProcessNode(pScene, aiNode->mChildren[i]);
        pNode->AddChild(pChildNode);
    }

    return pNode;
}

Canavar::Engine::MeshPtr Canavar::Engine::ModelImporter::ProcessMesh(aiMesh* aiMesh)
{
    MeshPtr pMesh = std::make_shared<Mesh>();

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex vertex;
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

    return pMesh;
}

Canavar::Engine::MaterialPtr Canavar::Engine::ModelImporter::ProcessMaterial(aiMaterial* aiMaterial, const QString& directory)
{
    MaterialPtr pMaterial = std::make_shared<Material>();

    if (!ProcessTexture(pMaterial, aiMaterial, aiTextureType_AMBIENT, TextureType::Ambient, directory))
    {
        ProcessTexture(pMaterial, aiMaterial, aiTextureType_BASE_COLOR, TextureType::Ambient, directory);
    }

    ProcessTexture(pMaterial, aiMaterial, aiTextureType_DIFFUSE, TextureType::Diffuse, directory);
    ProcessTexture(pMaterial, aiMaterial, aiTextureType_SPECULAR, TextureType::Specular, directory);
    ProcessTexture(pMaterial, aiMaterial, aiTextureType_HEIGHT, TextureType::Normal, directory);

    return pMaterial;
}

bool Canavar::Engine::ModelImporter::ProcessTexture(MaterialPtr pMaterial, aiMaterial* aiMaterial, aiTextureType aiType, TextureType type, const QString& directory)
{
    bool success = false;

    for (int i = 0; i < qMin(1, static_cast<int>(aiMaterial->GetTextureCount(aiType))); i++)
    {
        aiString str;
        aiMaterial->GetTexture(aiType, i, &str);
        QString filename = QString(str.C_Str());
        const auto path = directory + "/" + filename;

        if (const auto pTexture = CreateTexture(path))
        {
            pMaterial->SetTexture(type, pTexture);
            success = true;
        }
    }

    return success;
}

QOpenGLTexture* Canavar::Engine::ModelImporter::CreateTexture(const QString& path)
{
    QImage image(path);

    if (image.isNull())
    {
        LOG_WARN("ModelImporter::CreateTexture: Image at '{}' is null.", path.toStdString());
        return nullptr;
    }

    QOpenGLTexture* pTexture = new QOpenGLTexture(image, QOpenGLTexture::GenerateMipMaps);
    pTexture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    pTexture->setMinMagFilters(QOpenGLTexture::Filter::LinearMipMapLinear, QOpenGLTexture::Filter::Linear);

    return pTexture;
}
