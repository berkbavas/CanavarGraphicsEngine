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

    object.insert("color_r", mColor.x());
    object.insert("color_g", mColor.y());
    object.insert("color_b", mColor.z());
    object.insert("color.a", mColor.w());

    object.insert("ambient", mAmbient);
    object.insert("diffuse", mDiffuse);
    object.insert("specular", mSpecular);
    object.insert("shininess", mShininess);

    object.insert("invert_normals", mInvertNormals);

    // TODO: mMeshTransformations
}

void Canavar::Engine::Model::FromJson(const QJsonObject& object)
{
    Object::FromJson(object);

    float r = object["color_r"].toDouble(1.0f);
    float g = object["color_g"].toDouble(1.0f);
    float b = object["color_b"].toDouble(1.0f);
    float a = object["color_a"].toDouble(1.0f);
    mColor = QVector4D(r, g, b, a);

    mAmbient = object["ambient"].toDouble(0.25f);
    mDiffuse = object["diffuse"].toDouble(0.75f);
    mSpecular = object["specular"].toDouble(0.25f);
    mShininess = object["shininess"].toDouble(8.0f);

    mInvertNormals = object["invert_normals"].toBool(false);
}
