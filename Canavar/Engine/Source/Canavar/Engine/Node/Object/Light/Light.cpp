#include "Light.h"

void Canavar::Engine::Light::ToJson(QJsonObject &object)
{
    Object::ToJson(object);

    QJsonObject color;
    color.insert("r", mColor.x());
    color.insert("g", mColor.y());
    color.insert("b", mColor.z());
    color.insert("a", mColor.w());
    object.insert("color", color);

    object.insert("ambient", mAmbient);
    object.insert("diffuse", mDiffuse);
    object.insert("specular", mSpecular);
    object.insert("enabled", mEnabled);
}

void Canavar::Engine::Light::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    Object::FromJson(object, nodes);

    float r = object["color"]["r"].toDouble();
    float g = object["color"]["g"].toDouble();
    float b = object["color"]["b"].toDouble();
    float a = object["color"]["a"].toDouble(1.0f);

    mColor = QVector4D(r, g, b, a);

    mAmbient = object["ambient"].toDouble(1.0f);
    mDiffuse = object["diffuse"].toDouble(1.0f);
    mSpecular = object["specular"].toDouble(1.0f);

    mEnabled = object["enabled"].toBool(true);
}
