#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector4D>

class Material
{
public:
    Material();

    float ambient() const;
    void setAmbient(float newAmbient);

    float diffuse() const;
    void setDiffuse(float newDiffuse);

    float specular() const;
    void setSpecular(float newSpecular);

    float shininess() const;
    void setShininess(float newShininess);

    const QVector4D &color() const;
    void setColor(const QVector4D &newColor);

private:
    float mAmbient;
    float mDiffuse;
    float mSpecular;
    float mShininess;
    QVector4D mColor;
};

#endif // MATERIAL_H
