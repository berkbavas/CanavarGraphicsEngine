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

    object.insert("useColor", mUseColor);

    object.insert("metallic", mMetallic);
    object.insert("roughness", mRoughness);
    object.insert("ambient_occlusion", mAmbientOcclusion);

    object.insert("invert_normals", mInvertNormals);
    object.insert("scene_name", mSceneName);

    // TODO: mMeshTransformations
}

void Canavar::Engine::Model::FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes)
{
    Object::FromJson(object, nodes);

    QJsonObject color = object["color"].toObject();
    float r = color["r"].toDouble(1.0f);
    float g = color["g"].toDouble(1.0f);
    float b = color["b"].toDouble(1.0f);
    mColor = QVector3D(r, g, b);

    mUseColor = object["useColor"].toBool(false);

    mMetallic = object["metallic"].toDouble(0.0f);
    mRoughness = object["roughness"].toDouble(0.0f);
    mAmbientOcclusion = object["ambient_occlusion"].toDouble(2.0f);

    mInvertNormals = object["invert_normals"].toBool(false);
    mSceneName = object["scene_name"].toString();
}
