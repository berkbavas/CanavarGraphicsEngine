#include "Text2D.h"

Canavar::Engine::Text2D::Text2D()
{
    SetNodeName("Text2D");
}

void Canavar::Engine::Text2D::ToJson(QJsonObject &object)
{
    Node::ToJson(object);

    object.insert("color", QJsonValue::fromVariant(mColor));
    object.insert("position", QJsonValue::fromVariant(mPosition));
    object.insert("scale", mScale);
    object.insert("text", mText);
}

void Canavar::Engine::Text2D::FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes)
{
    Node::FromJson(object, nodes);
    mColor = object["color"].toVariant().value<QVector3D>();
    mPosition = object["position"].toVariant().value<QVector2D>();
    mScale = object["scale"].toVariant().value<float>();
    mText = object["text"].toString();
}
