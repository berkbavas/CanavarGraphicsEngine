#include "Text3D.h"

Canavar::Engine::Text3D::Text3D()
{
    SetNodeName("Text3D");
}

void Canavar::Engine::Text3D::ToJson(QJsonObject &object)
{
    Object::ToJson(object);

    QJsonObject color;
    color.insert("r", mColor.x());
    color.insert("g", mColor.y());
    color.insert("b", mColor.z());
    object.insert("color", color);

    object.insert("text", mText);
}

void Canavar::Engine::Text3D::FromJson(const QJsonObject &object, const QSet<NodePtr> &nodes)
{
    Object::FromJson(object, nodes);

    mColor = QVector3D(object["color"].toObject().value("r").toDouble(), //
                       object["color"].toObject().value("g").toDouble(), //
                       object["color"].toObject().value("b").toDouble());

    mText = object["text"].toString();
}
