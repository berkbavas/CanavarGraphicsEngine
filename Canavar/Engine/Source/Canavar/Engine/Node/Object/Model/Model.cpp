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
    color.insert("a", mColor.w());
    object.insert("color", color);

    object.insert("useColor", mUseColor);
    object.insert("ambient", mAmbient);
    object.insert("diffuse", mDiffuse);
    object.insert("specular", mSpecular);
    object.insert("shininess", mShininess);

    object.insert("invert_normals", mInvertNormals);
    object.insert("scene_name", mSceneName);

    // TODO: mMeshTransformations
}

void Canavar::Engine::Model::FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes)
{
    Object::FromJson(object, nodes);

    QJsonObject color = object["color"].toObject();
    float r = object["r"].toDouble(1.0f);
    float g = object["g"].toDouble(1.0f);
    float b = object["b"].toDouble(1.0f);
    float a = object["a"].toDouble(1.0f);
    mColor = QVector4D(r, g, b, a);

    mUseColor = object["useColor"].toBool(false);

    mAmbient = object["ambient"].toDouble(0.25f);
    mDiffuse = object["diffuse"].toDouble(0.75f);
    mSpecular = object["specular"].toDouble(0.25f);
    mShininess = object["shininess"].toDouble(8.0f);

    mInvertNormals = object["invert_normals"].toBool(false);
    mSceneName = object["scene_name"].toString();
}
