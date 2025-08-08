#include "Text2D.h"

Canavar::Engine::Text2D::Text2D()
{
    SetNodeName("Text2D");
}

void Canavar::Engine::Text2D::ToJson(QJsonObject &object)
{
    Node::ToJson(object);

    QJsonObject color;
    color.insert("r", mColor.x());
    color.insert("g", mColor.y());
    color.insert("b", mColor.z());
    object.insert("color", color);

    QJsonObject position;
    position.insert("x", mPosition.x());
    position.insert("y", mPosition.y());
    object.insert("position", position);

    object.insert("scale", mScale);
    object.insert("text", mText);
}

void Canavar::Engine::Text2D::FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes)
{
    Node::FromJson(object, nodes);

    mColor = QVector3D(object["color"].toObject().value("r").toDouble(), //
                       object["color"].toObject().value("g").toDouble(), //
                       object["color"].toObject().value("b").toDouble());

    mPosition = QVector2D(object["position"].toObject().value("x").toDouble(), //
                          object["position"].toObject().value("y").toDouble());

    mScale = object["scale"].toVariant().value<float>();
    mText = object["text"].toString();
}
