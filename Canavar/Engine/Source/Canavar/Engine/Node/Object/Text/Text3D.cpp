#include "Text3D.h"

Canavar::Engine::Text3D::Text3D()
{
    SetNodeName("Text3D");
}

void Canavar::Engine::Text3D::ToJson(QJsonObject &object)
{
    Object::ToJson(object);

    object.insert("color", QJsonValue::fromVariant(mColor));
    object.insert("text", mText);
}

void Canavar::Engine::Text3D::FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes)
{
    Object::FromJson(object, nodes);

    mColor = object["color"].toVariant().value<QVector3D>();
    mText = object["text"].toString();
}
