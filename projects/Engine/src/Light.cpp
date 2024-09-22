#include "Light.h"

Canavar::Engine::Light::Light()
    : Node()
    , mColor(1.0f, 1.0f, 1.0f, 1.0f)
    , mAmbient(1.0f)
    , mDiffuse(1.0f)
    , mSpecular(1.0f)
{}

void Canavar::Engine::Light::ToJson(QJsonObject& object)
{
    Node::ToJson(object);

    QJsonObject color;
    color.insert("r", mColor.x());
    color.insert("g", mColor.y());
    color.insert("b", mColor.z());
    color.insert("a", mColor.w());
    object.insert("color", color);

    object.insert("ambient", mAmbient);
    object.insert("diffuse", mDiffuse);
    object.insert("specular", mSpecular);
}

void Canavar::Engine::Light::FromJson(const QJsonObject& object)
{
    Node::FromJson(object);

    // Color
    {
        float r = object["color"]["r"].toDouble();
        float g = object["color"]["g"].toDouble();
        float b = object["color"]["b"].toDouble();
        float a = object["color"]["a"].toDouble();

        mColor = QVector4D(r, g, b, a);
    }

    mAmbient = object["ambient"].toDouble();
    mDiffuse = object["diffuse"].toDouble();
    mSpecular = object["specular"].toDouble();
}