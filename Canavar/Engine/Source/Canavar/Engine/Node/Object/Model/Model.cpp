#include "Model.h"

Canavar::Engine::Model::Model(const QString& sceneName)
    : mSceneName(sceneName)
{
    SetNodeName(sceneName);
}

QMatrix4x4 Canavar::Engine::Model::GetMeshTransformation(const QString& meshName)
{
    return mMeshTransformations.value(meshName, QMatrix4x4());
}

void Canavar::Engine::Model::SetMeshTransformation(const QString& meshName, const QMatrix4x4& transformation)
{
    mMeshTransformations.insert(meshName, transformation);
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
    object.insert("scene_name", mSceneName);
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
    mSceneName = object["scene_name"].toString();
}
