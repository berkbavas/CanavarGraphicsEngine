#include "Model.h"

#include "Canavar/Engine/Node/NodeVisitor.h"

#include <QJsonArray>

Canavar::Engine::Model::Model(const std::string& SceneName)
    : mSceneName(SceneName)
{
    SetNodeName(SceneName);
}

QMatrix4x4 Canavar::Engine::Model::GetMeshTransformation(const std::string& MeshName) const
{
    return mMeshTransformations.value(MeshName, QMatrix4x4());
}

void Canavar::Engine::Model::SetMeshTransformation(const std::string& MeshName, const QMatrix4x4& Transformation)
{
    mMeshTransformations.insert(MeshName, Transformation);
}

bool Canavar::Engine::Model::GetMeshVisibility(const std::string& MeshName) const
{
    return mMeshVisibilities.value(MeshName, true);
}

void Canavar::Engine::Model::SetMeshVisibility(const std::string& MeshName, bool Visible)
{
    mMeshVisibilities.insert(MeshName, Visible);
}

float Canavar::Engine::Model::GetMeshOpacity(const std::string& MeshName, float DefaultOpacity) const
{
    return mMeshOpacities.value(MeshName, DefaultOpacity);
}

void Canavar::Engine::Model::SetMeshOpacity(const std::string& MeshName, float Opacity)
{
    mMeshOpacities.insert(MeshName, Opacity);
}

void Canavar::Engine::Model::ToJson(QJsonObject& Object)
{
    Object::ToJson(Object);

    QJsonObject Color;
    Color.insert("r", mColor.x());
    Color.insert("g", mColor.y());
    Color.insert("b", mColor.z());
    Object.insert("color", Color);

    Object.insert("use_model_color", mUseModelColor);
    Object.insert("scene_name", QString::fromStdString(mSceneName));

    // Serialize mesh transformations
    QJsonObject MeshTransformationsJson;
    for (auto Iterator = mMeshTransformations.constBegin(); Iterator != mMeshTransformations.constEnd(); ++Iterator)
    {
        QJsonArray MatrixArray;
        const float* data = Iterator.value().constData();
        for (int Index = 0; Index < 16; ++Index)
        {
            MatrixArray.append(data[Index]);
        }
        MeshTransformationsJson.insert(QString::fromStdString(Iterator.key()), MatrixArray);
    }
    Object.insert("mesh_transformations", MeshTransformationsJson);

    // Serialize mesh visibilities
    QJsonObject MeshVisibilitiesJson;
    for (auto Iterator = mMeshVisibilities.constBegin(); Iterator != mMeshVisibilities.constEnd(); ++Iterator)
    {
        MeshVisibilitiesJson.insert(QString::fromStdString(Iterator.key()), Iterator.value());
    }
    Object.insert("mesh_visibilities", MeshVisibilitiesJson);

    // Serialize mesh opacities
    QJsonObject MeshOpacitiesJson;
    for (auto Iterator = mMeshOpacities.constBegin(); Iterator != mMeshOpacities.constEnd(); ++Iterator)
    {
        MeshOpacitiesJson.insert(QString::fromStdString(Iterator.key()), Iterator.value());
    }
    Object.insert("mesh_opacities", MeshOpacitiesJson);
}

void Canavar::Engine::Model::FromJson(const QJsonObject& Object, const QSet<NodePtr>& Nodes)
{
    Object::FromJson(Object, Nodes);

    QJsonObject Color = Object["color"].toObject();
    float r = Color["r"].toDouble(1.0f);
    float g = Color["g"].toDouble(1.0f);
    float b = Color["b"].toDouble(1.0f);
    mColor = QVector3D(r, g, b);

    mUseModelColor = Object["use_model_color"].toBool(false);
    mSceneName = Object["scene_name"].toString().toStdString();

    // Deserialize mesh transformations
    mMeshTransformations.clear();
    QJsonObject MeshTransformationsJson = Object["mesh_transformations"].toObject();
    for (auto Iterator = MeshTransformationsJson.constBegin(); Iterator != MeshTransformationsJson.constEnd(); ++Iterator)
    {
        QJsonArray MatrixArray = Iterator.value().toArray();
        if (MatrixArray.size() == 16)
        {
            float Data[16];
            for (int Index = 0; Index < 16; ++Index)
            {
                Data[Index] = static_cast<float>(MatrixArray[Index].toDouble());
            }
            QMatrix4x4 Matrix(Data);
            mMeshTransformations.insert(Iterator.key().toStdString(), Matrix);
        }
    }

    // Deserialize mesh visibilities
    mMeshVisibilities.clear();
    QJsonObject MeshVisibilitiesJson = Object["mesh_visibilities"].toObject();
    for (auto Iterator = MeshVisibilitiesJson.constBegin(); Iterator != MeshVisibilitiesJson.constEnd(); ++Iterator)
    {
        mMeshVisibilities.insert(Iterator.key().toStdString(), Iterator.value().toBool(true));
    }

    // Deserialize mesh opacities
    mMeshOpacities.clear();
    QJsonObject MeshOpacitiesJson = Object["mesh_opacities"].toObject();
    for (auto Iterator = MeshOpacitiesJson.constBegin(); Iterator != MeshOpacitiesJson.constEnd(); ++Iterator)
    {
        mMeshOpacities.insert(Iterator.key().toStdString(), static_cast<float>(Iterator.value().toDouble(1.0)));
    }
}
