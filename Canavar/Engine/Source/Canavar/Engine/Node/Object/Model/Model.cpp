#include "Model.h"

#include "Canavar/Engine/Node/NodeVisitor.h"

#include <QJsonArray>

Canavar::Engine::Model::Model(const std::string& sceneName)
    : mSceneName(sceneName)
{
    SetNodeName(sceneName);
}

QMatrix4x4 Canavar::Engine::Model::GetMeshTransformation(const std::string& meshName) const
{
    return mMeshTransformations.value(meshName, QMatrix4x4());
}

void Canavar::Engine::Model::SetMeshTransformation(const std::string& meshName, const QMatrix4x4& transformation)
{
    mMeshTransformations.insert(meshName, transformation);
}

bool Canavar::Engine::Model::GetMeshVisibility(const std::string& meshName) const
{
    return mMeshVisibilities.value(meshName, true);
}

void Canavar::Engine::Model::SetMeshVisibility(const std::string& meshName, bool visible)
{
    mMeshVisibilities.insert(meshName, visible);
}

float Canavar::Engine::Model::GetMeshOpacity(const std::string& meshName, float defaultOpacity) const
{
    return mMeshOpacities.value(meshName, defaultOpacity);
}

void Canavar::Engine::Model::SetMeshOpacity(const std::string& meshName, float opacity)
{
    mMeshOpacities.insert(meshName, opacity);
}

void Canavar::Engine::Model::ToJson(QJsonObject& object)
{
    Object::ToJson(object);

    QJsonObject color;
    color.insert("r", mColor.x());
    color.insert("g", mColor.y());
    color.insert("b", mColor.z());
    object.insert("color", color);

    object.insert("use_model_color", mUseModelColor);
    object.insert("scene_name", QString::fromStdString(mSceneName));

    // Serialize mesh transformations
    QJsonObject MeshTransformationsJson;
    for (auto it = mMeshTransformations.constBegin(); it != mMeshTransformations.constEnd(); ++it)
    {
        QJsonArray MatrixArray;
        const float* data = it.value().constData();
        for (int i = 0; i < 16; ++i)
        {
            MatrixArray.append(data[i]);
        }
        MeshTransformationsJson.insert(QString::fromStdString(it.key()), MatrixArray);
    }
    object.insert("mesh_transformations", MeshTransformationsJson);

    // Serialize mesh visibilities
    QJsonObject MeshVisibilitiesJson;
    for (auto it = mMeshVisibilities.constBegin(); it != mMeshVisibilities.constEnd(); ++it)
    {
        MeshVisibilitiesJson.insert(QString::fromStdString(it.key()), it.value());
    }
    object.insert("mesh_visibilities", MeshVisibilitiesJson);

    // Serialize mesh opacities
    QJsonObject MeshOpacitiesJson;
    for (auto it = mMeshOpacities.constBegin(); it != mMeshOpacities.constEnd(); ++it)
    {
        MeshOpacitiesJson.insert(QString::fromStdString(it.key()), it.value());
    }
    object.insert("mesh_opacities", MeshOpacitiesJson);
}

void Canavar::Engine::Model::FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes)
{
    Object::FromJson(object, nodes);

    QJsonObject color = object["color"].toObject();
    float r = color["r"].toDouble(1.0f);
    float g = color["g"].toDouble(1.0f);
    float b = color["b"].toDouble(1.0f);
    mColor = QVector3D(r, g, b);

    mUseModelColor = object["use_model_color"].toBool(false);
    mSceneName = object["scene_name"].toString().toStdString();

    // Deserialize mesh transformations
    mMeshTransformations.clear();
    QJsonObject MeshTransformationsJson = object["mesh_transformations"].toObject();
    for (auto it = MeshTransformationsJson.constBegin(); it != MeshTransformationsJson.constEnd(); ++it)
    {
        QJsonArray MatrixArray = it.value().toArray();
        if (MatrixArray.size() == 16)
        {
            float Data[16];
            for (int i = 0; i < 16; ++i)
            {
                Data[i] = static_cast<float>(MatrixArray[i].toDouble());
            }
            QMatrix4x4 Matrix(Data);
            mMeshTransformations.insert(it.key().toStdString(), Matrix);
        }
    }

    // Deserialize mesh visibilities
    mMeshVisibilities.clear();
    QJsonObject MeshVisibilitiesJson = object["mesh_visibilities"].toObject();
    for (auto it = MeshVisibilitiesJson.constBegin(); it != MeshVisibilitiesJson.constEnd(); ++it)
    {
        mMeshVisibilities.insert(it.key().toStdString(), it.value().toBool(true));
    }

    // Deserialize mesh opacities
    mMeshOpacities.clear();
    QJsonObject MeshOpacitiesJson = object["mesh_opacities"].toObject();
    for (auto it = MeshOpacitiesJson.constBegin(); it != MeshOpacitiesJson.constEnd(); ++it)
    {
        mMeshOpacities.insert(it.key().toStdString(), static_cast<float>(it.value().toDouble(1.0)));
    }
}
