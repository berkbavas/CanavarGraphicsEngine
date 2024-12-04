#include "Haze.h"

Canavar::Engine::Haze::Haze()
{
    SetNodeName("Haze");
}

void Canavar::Engine::Haze::ToJson(QJsonObject &object)
{
    GlobalNode::ToJson(object);

    QJsonObject color;
    color.insert("r", mColor.x());
    color.insert("g", mColor.y());
    color.insert("b", mColor.z());
    object.insert("color", color);

    object.insert("density", mDensity);
    object.insert("gradient", mGradient);
}

void Canavar::Engine::Haze::FromJson(const QJsonObject &object, const std::map<QString, NodePtr> &nodes)
{
    GlobalNode::FromJson(object, nodes);

    float x = object["color"]["r"].toDouble(0.33f);
    float y = object["color"]["g"].toDouble(0.38f);
    float z = object["color"]["b"].toDouble(0.47f);
    mColor = QVector3D(x, y, z);

    mDensity = object["density"].toDouble(1.0f);
    mGradient = object["gradient"].toDouble(1.5f);
}
