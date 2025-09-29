#include "Light.h"

void Canavar::Engine::Light::ToJson(QJsonObject &Object)
{
    Object::ToJson(Object);

    QJsonObject Color;
    Color.insert("r", mColor.x());
    Color.insert("g", mColor.y());
    Color.insert("b", mColor.z());
    Object.insert("color", Color);

    Object.insert("ambient", mAmbient);
    Object.insert("diffuse", mDiffuse);
    Object.insert("specular", mSpecular);
    Object.insert("enabled", mEnabled);
}

void Canavar::Engine::Light::FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes)
{
    Object::FromJson(Object, Nodes);

    float r = Object["color"]["r"].toDouble();
    float g = Object["color"]["g"].toDouble();
    float b = Object["color"]["b"].toDouble();

    mColor = QVector3D(r, g, b);

    mAmbient = Object["ambient"].toDouble(1.0f);
    mDiffuse = Object["diffuse"].toDouble(1.0f);
    mSpecular = Object["specular"].toDouble(1.0f);

    mEnabled = Object["enabled"].toBool(true);
}
