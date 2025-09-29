#include "Text3D.h"

#include "Canavar/Engine/Node/NodeVisitor.h"

void Canavar::Engine::Text3D::ToJson(QJsonObject &Object)
{
    Object::ToJson(Object);

    QJsonObject Color;
    Color.insert("r", mColor.x());
    Color.insert("g", mColor.y());
    Color.insert("b", mColor.z());
    Object.insert("color", Color);

    Object.insert("text", mText);
}

void Canavar::Engine::Text3D::FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes)
{
    Object::FromJson(Object, Nodes);

    mColor = QVector3D(Object["color"].toObject().value("r").toDouble(), //
                       Object["color"].toObject().value("g").toDouble(), //
                       Object["color"].toObject().value("b").toDouble());

    mText = Object["text"].toString();
}
