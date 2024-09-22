#include "Helper.h"
#include "ModelData.h"

#include <QFile>
#include <QJsonDocument>
#include <QMultiMap>
#include <QtMath>

#include <limits>

Canavar::Engine::Helper::Helper() {}

float Canavar::Engine::Helper::CalculateHorizontalFovForGivenVerticalFov(float verticalFov, float width, float height)
{
    float hvfr = 0.5f * qDegreesToRadians(verticalFov);
    float hfr = 2 * atan(tan(hvfr) / (width / height));

    return qRadiansToDegrees(hfr);
}

float Canavar::Engine::Helper::CalculateVerticalFovForGivenHorizontalFov(float horizontalFov, float width, float height)
{
    float hhfr = 0.5f * qDegreesToRadians(horizontalFov);
    float vfr = 2 * atan(tan(hhfr) * (width / height));

    return qRadiansToDegrees(vfr);
}

QByteArray Canavar::Engine::Helper::GetBytes(QString path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly))
    {
        return file.readAll();
    }
    else
    {
        qWarning() << QString("Could not open '%1'").arg(path);
        return QByteArray();
    }
}

QQuaternion Canavar::Engine::Helper::RotateX(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Canavar::Engine::Helper::RotateY(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Canavar::Engine::Helper::RotateZ(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), qRadiansToDegrees(angleRadians));
}

float Canavar::Engine::Helper::AngleBetween(const QVector3D& v1, const QVector3D& v2, const QVector3D& left)
{
    QVector3D u1 = v1.normalized();
    QVector3D u2 = v2.normalized();

    float dot = QVector3D::dotProduct(u1, u2);

    if (qFuzzyCompare(dot, 1.0f))
    {
        return 0.0f;
    }
    else if (qFuzzyCompare(dot, -1.0f))
    {
        return 180.0f;
    }
    else
    {
        float angle = qRadiansToDegrees(acos(dot));
        QVector3D u1xu2 = QVector3D::crossProduct(u1, u2);

        if (QVector3D::dotProduct(u1xu2, left) < 0.0001f)
            return angle;
        else
            return -angle;
    }
}

float Canavar::Engine::Helper::AngleBetween(const QVector3D& v1, const QVector3D& v2)
{
    QVector3D u1 = v1.normalized();
    QVector3D u2 = v2.normalized();

    float dot = QVector3D::dotProduct(u1, u2);

    if (qFuzzyCompare(dot, 1.0f))
    {
        return 0.0f;
    }
    else if (qFuzzyCompare(dot, -1.0f))
    {
        return 180.0f;
    }
    else
    {
        return qRadiansToDegrees(acos(dot));
    }
}

void Canavar::Engine::Helper::GetEulerDegrees(const QQuaternion& rotation, float& yaw, float& pitch, float& roll)
{
    QVector3D zAxis = rotation * QVector3D(0, 0, -1);
    float x = zAxis.x();
    float y = zAxis.y();
    float z = zAxis.z();
    yaw = qRadiansToDegrees(atan2(-z, x)) - 90;
    pitch = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

    QVector3D xAxis = rotation * QVector3D(1, 0, 0);

    QVector3D xAxisProj = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
        QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
        QVector3D(1, 0, 0);

    QVector3D left = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
        QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
        QVector3D(0, 0, -1);

    roll = Canavar::Engine::Helper::AngleBetween(xAxis, xAxisProj, left);

    if (yaw < 0.0f)
        yaw += 360.0f;
}

QQuaternion Canavar::Engine::Helper::ConstructFromEulerDegrees(float yaw, float pitch, float roll)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
        QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
        QQuaternion::fromAxisAndAngle(QVector3D(0, 0, -1), roll);
}

QQuaternion Canavar::Engine::Helper::Invert(const QQuaternion& rotation)
{
    float yaw, pitch, roll;
    GetEulerDegrees(rotation, yaw, pitch, roll);

    QQuaternion r = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw);
    r = r * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -pitch);
    r = r * QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), roll);
    return r;
}

Canavar::Engine::ModelData* Canavar::Engine::Helper::LoadModel(const QString& name, const QString& path)
{
    Assimp::Importer importer;

    auto* aiScene = importer.ReadFile(path.toStdString(),              //
        aiProcess_Triangulate |          //
        aiProcess_GenSmoothNormals | //
        aiProcess_FlipUVs |          //
        aiProcess_CalcTangentSpace | //
        aiProcess_GenBoundingBoxes);

    if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
    {
        qCritical() << Q_FUNC_INFO << importer.GetErrorString();
        return nullptr;
    }

    QString directory = path.left(path.lastIndexOf("/"));

    ModelData* data = new ModelData(name);

    // Node
    ModelDataNode* root = ProcessNode(data, aiScene->mRootNode);
    data->SetRootNode(root);

    // Materials
    for (unsigned int i = 0; i < aiScene->mNumMaterials; ++i)
    {
        Material* material = ProcessMaterial(aiScene->mMaterials[i], directory);
        data->AddMaterial(material);
    }

    // Meshes
    for (unsigned int i = 0; i < aiScene->mNumMeshes; i++)
    {
        aiMesh* aiMesh = aiScene->mMeshes[i];
        Mesh* mesh = ProcessMesh(aiMesh);
        mesh->SetID(i);
        mesh->SetMaterial(data->GetMaterial(aiMesh->mMaterialIndex));
        data->AddMesh(mesh);
    }

    CalculateAABB(data);

    return data;
}

Canavar::Engine::Mesh* Canavar::Engine::Helper::ProcessMesh(aiMesh* aiMesh)
{
    Mesh* mesh = new Mesh;

    for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;

        vertex.position = QVector3D(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);

        if (aiMesh->HasNormals())
            vertex.normal = QVector3D(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z);

        if (aiMesh->mTextureCoords[0])
            vertex.texture = QVector2D(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);

        if (aiMesh->HasTangentsAndBitangents())
        {
            vertex.tangent = QVector3D(aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z);
            vertex.bitangent = QVector3D(aiMesh->mBitangents[i].x, aiMesh->mBitangents[i].y, aiMesh->mBitangents[i].z);
        }

        mesh->AddVertex(vertex);
    }

    for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
    {
        aiFace aiFace = aiMesh->mFaces[i];

        for (unsigned int j = 0; j < aiFace.mNumIndices; j++)
            mesh->AddIndex(aiFace.mIndices[j]);
    }

    mesh->SetName(aiMesh->mName.C_Str());

    AABB aabb;
    auto min = aiMesh->mAABB.mMin;
    auto max = aiMesh->mAABB.mMax;
    aabb.SetMin(QVector3D(min.x, min.y, min.z));
    aabb.SetMax(QVector3D(max.x, max.y, max.z));
    mesh->SetAABB(aabb);

    return mesh;
}

Canavar::Engine::ModelDataNode* Canavar::Engine::Helper::ProcessNode(ModelData* data, aiNode* aiParent)
{
    ModelDataNode* parent = new ModelDataNode(data);

    for (unsigned int i = 0; i < aiParent->mNumMeshes; ++i)
        parent->AddMeshIndex(aiParent->mMeshes[i]);

    parent->SetName(aiParent->mName.C_Str());
    parent->SetWorldTransformation(ToQMatrix(aiParent->mTransformation));

    for (unsigned int i = 0; i < aiParent->mNumChildren; ++i)
    {
        auto child = ProcessNode(data, aiParent->mChildren[i]);
        parent->AddChild(child);
    }

    return parent;
}

Canavar::Engine::Material* Canavar::Engine::Helper::ProcessMaterial(aiMaterial* aiMaterial, const QString& directory)
{
    Material* material = new Material;
    if (!ProcessTexture(material, aiMaterial, aiTextureType_AMBIENT, Material::TextureType::Ambient, directory))
        ProcessTexture(material, aiMaterial, aiTextureType_BASE_COLOR, Material::TextureType::Ambient, directory);

    ProcessTexture(material, aiMaterial, aiTextureType_DIFFUSE, Material::TextureType::Diffuse, directory);
    ProcessTexture(material, aiMaterial, aiTextureType_SPECULAR, Material::TextureType::Specular, directory);
    ProcessTexture(material, aiMaterial, aiTextureType_HEIGHT, Material::TextureType::Normal, directory);

    return material;
}

bool Canavar::Engine::Helper::ProcessTexture(Material* material, aiMaterial* aiMaterial, aiTextureType aiType, Material::TextureType type, const QString& directory)
{
    bool success = false;

    for (int i = 0; i < qMin(1, int(aiMaterial->GetTextureCount(aiType))); i++)
    {
        aiString str;
        aiMaterial->GetTexture(aiType, i, &str);
        QString filename = QString(str.C_Str());
        auto path = directory + "/" + filename;

        if (auto texture = CreateTexture(path))
        {
            material->Insert(type, texture);
            success = true;
        }
    }

    return success;
}

QMatrix4x4 Canavar::Engine::Helper::ToQMatrix(const aiMatrix4x4& matrix)
{
    aiVector3D scaling;
    aiQuaternion rotation;
    aiVector3D position;
    matrix.Decompose(scaling, rotation, position);

    QVector3D qScaling = QVector3D(scaling.x, scaling.y, scaling.z);
    QQuaternion qRotation = QQuaternion(rotation.w, rotation.x, rotation.y, rotation.z);
    QVector4D qPosition = QVector4D(position.x, position.y, position.z, 1.0);

    QMatrix4x4 transformation;

    transformation.scale(qScaling);
    transformation.rotate(qRotation);
    transformation.setColumn(3, qPosition);

    return transformation;
}

void Canavar::Engine::Helper::CalculateAABB(ModelData* data)
{
    float minX = std::numeric_limits<float>::infinity();
    float minY = std::numeric_limits<float>::infinity();
    float minZ = std::numeric_limits<float>::infinity();

    float maxX = -std::numeric_limits<float>::infinity();
    float maxY = -std::numeric_limits<float>::infinity();
    float maxZ = -std::numeric_limits<float>::infinity();

    for (const auto& mesh : data->GetMeshes())
    {
        if (minX > mesh->GetAABB().GetMin().x())
            minX = mesh->GetAABB().GetMin().x();

        if (minY > mesh->GetAABB().GetMin().y())
            minY = mesh->GetAABB().GetMin().y();

        if (minZ > mesh->GetAABB().GetMin().z())
            minZ = mesh->GetAABB().GetMin().z();

        if (maxX < mesh->GetAABB().GetMax().x())
            maxX = mesh->GetAABB().GetMax().x();

        if (maxY < mesh->GetAABB().GetMax().y())
            maxY = mesh->GetAABB().GetMax().y();

        if (maxZ < mesh->GetAABB().GetMax().z())
            maxZ = mesh->GetAABB().GetMax().z();
    }

    AABB aabb;
    aabb.SetMin(QVector3D(minX, minY, minZ));
    aabb.SetMax(QVector3D(maxX, maxY, maxZ));
    data->SetAABB(aabb);
}

float Canavar::Engine::Helper::GenerateFloat(float bound)
{
    return mGenerator.bounded(bound);
}

float Canavar::Engine::Helper::GenerateBetween(float lower, float upper)
{
    return lower + mGenerator.bounded(upper - lower);
}

QVector3D Canavar::Engine::Helper::GenerateVec3(float x, float y, float z)
{
    return QVector3D(GenerateFloat(x), GenerateFloat(y), GenerateFloat(z));
}

QOpenGLTexture* Canavar::Engine::Helper::CreateTexture(const QString& path)
{
    QImage image(path);

    if (image.isNull())
    {
        qWarning() << "An image at " + path + " is null.";
        return nullptr;
    }

    QOpenGLTexture* texture = new QOpenGLTexture(image, QOpenGLTexture::GenerateMipMaps);
    texture->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    texture->setMinMagFilters(QOpenGLTexture::Filter::LinearMipMapLinear, QOpenGLTexture::Filter::Linear);

    return texture;
}

QVector<Canavar::Engine::PointLight*> Canavar::Engine::Helper::GetClosePointLights(const QList<PointLight*>& lights, const QVector3D& position, int maxCount)
{
    QMultiMap<float, PointLight*> distances;

    for (const auto& light : lights)
    {
        if (light->GetVisible())
            distances.insert((light->WorldPosition() - position).length(), light);
    }

    QList<PointLight*> nodesStorted = distances.values();

    QVector<PointLight*> result;

    for (int i = 0; i < qMin(maxCount, nodesStorted.size()); ++i)
        result << nodesStorted[i];

    return result;
}

QJsonDocument Canavar::Engine::Helper::LoadJson(const QString& path)
{
    QJsonDocument document;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "Error occured while reading the file:" << path;
        return document;
    }

    document = QJsonDocument::fromJson(file.readAll());
    file.close();

    return document;
}

bool Canavar::Engine::Helper::WriteTextToFile(const QString& path, const QByteArray& content)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream << content;
        stream.flush();
        file.close();
        return true;
    }
    else
    {
        qCritical() << Q_FUNC_INFO << "Couldn't write to file" << path;
        return false;
    }
}

bool Canavar::Engine::Helper::WriteDataToFile(const QString& path, const QByteArray& content)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream stream(&file);
        stream << content;
        file.close();
        return true;
    }
    else
    {
        qCritical() << Q_FUNC_INFO << "Couldn't write to file" << path;
        return false;
    }
}

QByteArray Canavar::Engine::Helper::ReadDataFromFile(const QString& path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray arr;
        QDataStream stream(&file);
        stream >> arr;
        file.close();
        return arr;
    }
    else
    {
        qCritical() << Q_FUNC_INFO << "Couldn't read from file" << path;
        return QByteArray();
    }
}

QRandomGenerator Canavar::Engine::Helper::mGenerator = QRandomGenerator::securelySeeded();