#include "ModelImporter.h"

#include "Canavar/Engine/Node/Object/Scene/AABB.h"
#include "Canavar/Engine/Util/Logger.h"

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
        pMesh->SetMeshId(i);
        pMesh->SetMaterial(pMaterial);
        pScene->AddMesh(pMesh);
    }

    // Nodes
    SceneNodePtr pRootNode = ProcessNode(pScene, aiScene->mRootNode);
    pScene->SetRootNode(pRootNode);

    // AABB
    float minX = std::numeric_limits<float>::infinity();
    float minY = std::numeric_limits<float>::infinity();
    float minZ = std::numeric_limits<float>::infinity();

    float maxX = -std::numeric_limits<float>::infinity();
    float maxY = -std::numeric_limits<float>::infinity();
    float maxZ = -std::numeric_limits<float>::infinity();

    const auto& meshes = pScene->GetMeshes();
    for (const auto& pMesh : meshes)
    {
        if (minX > pMesh->GetAABB().GetMin().x())
            minX = pMesh->GetAABB().GetMin().x();

        if (minY > pMesh->GetAABB().GetMin().y())
            minY = pMesh->GetAABB().GetMin().y();

        if (minZ > pMesh->GetAABB().GetMin().z())
            minZ = pMesh->GetAABB().GetMin().z();

        if (maxX < pMesh->GetAABB().GetMax().x())
            maxX = pMesh->GetAABB().GetMax().x();

        if (maxY < pMesh->GetAABB().GetMax().y())
            maxY = pMesh->GetAABB().GetMax().y();

        if (maxZ < pMesh->GetAABB().GetMax().z())
            maxZ = pMesh->GetAABB().GetMax().z();
    }

    pScene->SetAABB(AABB{ QVector3D(minX, minY, minZ), QVector3D(maxX, maxY, maxZ) });

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
    pNode->SetTransformation(ToQMatrix4x4(aiNode->mTransformation));

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
        vertex.position.setW(1.0f);

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

Canavar::Engine::MaterialPtr Canavar::Engine::ModelImporter::ProcessMaterial(aiMaterial* aiMaterial, const QString& directory)
{
    MaterialPtr pMaterial = std::make_shared<Material>();

    constexpr aiTextureType AMBIENT_TARGETS[] = { aiTextureType_AMBIENT, aiTextureType_BASE_COLOR, aiTextureType_UNKNOWN, aiTextureType_DIFFUSE };

    for (const auto target : AMBIENT_TARGETS)
    {
        if (ProcessTexture(pMaterial, aiMaterial, target, TextureType::Ambient, directory))
        {
            break;
        }
    }

    constexpr aiTextureType DIFFUSE_TARGETS[] = { aiTextureType_DIFFUSE, aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_AMBIENT };

    for (const auto target : DIFFUSE_TARGETS)
    {
        if (ProcessTexture(pMaterial, aiMaterial, target, TextureType::Diffuse, directory))
        {
            break;
        }
    }

    ProcessTexture(pMaterial, aiMaterial, aiTextureType_SPECULAR, TextureType::Specular, directory);
    ProcessTexture(pMaterial, aiMaterial, aiTextureType_NORMALS, TextureType::Normal, directory);

    return pMaterial;
}

bool Canavar::Engine::ModelImporter::ProcessTexture(MaterialPtr pMaterial, aiMaterial* aiMaterial, aiTextureType aiType, TextureType type, const QString& directory)
{
    for (int i = 0; i < aiMaterial->GetTextureCount(aiType); i++)
    {
        aiString str;
        aiMaterial->GetTexture(aiType, i, &str);

        QString filename(str.C_Str());

        const auto path = directory + "/" + filename;

        QImage image(path);

        if (image.isNull())
        {
            LOG_WARN("ModelImporter::ProcessTexture: Image at '{}' is null.", path.toStdString());
        }
        else
        {
            LOG_DEBUG("ModelImporter::ProcessTexture: Loading texture for {}", path.toStdString());
            pMaterial->LoadTexture(type, image);
            return true;
        }
    }

    return false;
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

QMatrix4x4 Canavar::Engine::ModelImporter::ToQMatrix4x4(const aiMatrix4x4& matrix)
{
    aiVector3D scaling;
    aiQuaternion rotation;
    aiVector3D position;
    matrix.Decompose(scaling, rotation, position);

    QMatrix4x4 transformation;

    transformation.scale(scaling.x, scaling.y, scaling.z);
    transformation.rotate(rotation.w, rotation.x, rotation.y, rotation.z);
    transformation.setColumn(3, QVector4D(position.x, position.y, position.z, 1.0));

    return transformation;
}
