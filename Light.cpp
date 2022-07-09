#include "Light.h"

Light::Light(QObject *parent)
    : Node{parent}
    , mColor(1.0f, 1.0f, 1.0f, 1.0f)
    , mAmbient(1.0f)
    , mDiffuse(1.0f)
    , mSpecular(1.0f)
{}

const QVector4D &Light::color() const
{
    return mColor;
}

void Light::setColor(const QVector4D &newColor)
{
    mColor = newColor;
}

float Light::ambient() const
{
    return mAmbient;
}

void Light::setAmbient(float newAmbient)
{
    mAmbient = newAmbient;
}

float Light::diffuse() const
{
    return mDiffuse;
}

void Light::setDiffuse(float newDiffuse)
{
    mDiffuse = newDiffuse;
}

float Light::specular() const
{
    return mSpecular;
}

void Light::setSpecular(float newSpecular)
{
    mSpecular = newSpecular;
}
