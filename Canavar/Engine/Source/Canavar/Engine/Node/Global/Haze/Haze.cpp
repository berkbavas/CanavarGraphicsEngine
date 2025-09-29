#include "Haze.h"

void Canavar::Engine::Haze::ToJson(QJsonObject &Object)
{
    Global::ToJson(Object);

    QJsonObject Color;
    Color.insert("r", mColor.x());
    Color.insert("g", mColor.y());
    Color.insert("b", mColor.z());
    Object.insert("color", Color);

    Object.insert("density", mDensity);
    Object.insert("gradient", mGradient);
    Object.insert("enabled", mEnabled);
}

void Canavar::Engine::Haze::FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes)
{
    Global::FromJson(Object, Nodes);

    float x = Object["color"]["r"].toDouble(0.33f);
    float y = Object["color"]["g"].toDouble(0.38f);
    float z = Object["color"]["b"].toDouble(0.47f);
    mColor = QVector3D(x, y, z);

    mDensity = Object["density"].toDouble(1.0f);
    mGradient = Object["gradient"].toDouble(1.5f);
    mEnabled = Object["enabled"].toBool(true);
}
