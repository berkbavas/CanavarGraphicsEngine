#include "Text2D.h"

void Canavar::Engine::Text2D::ToJson(QJsonObject &Object)
{
    Node::ToJson(Object);

    QJsonObject Color;
    Color.insert("r", mColor.x());
    Color.insert("g", mColor.y());
    Color.insert("b", mColor.z());
    Object.insert("color", Color);

    QJsonObject Position;
    Position.insert("x", mPosition.x());
    Position.insert("y", mPosition.y());
    Object.insert("position", Position);

    Object.insert("scale", mScale);
    Object.insert("text", mText);
}

void Canavar::Engine::Text2D::FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes)
{
    Node::FromJson(Object, Nodes);

    mColor = QVector3D(Object["color"].toObject().value("r").toDouble(), //
                       Object["color"].toObject().value("g").toDouble(), //
                       Object["color"].toObject().value("b").toDouble());

    mPosition = QVector2D(Object["position"].toObject().value("x").toDouble(), //
                          Object["position"].toObject().value("y").toDouble());

    mScale = Object["scale"].toVariant().value<float>();
    mText = Object["text"].toString();
}
