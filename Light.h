#ifndef LIGHT_H
#define LIGHT_H

#include "Model.h"
#include "Node.h"

class Light : public Node
{
public:
    explicit Light(QObject *parent = nullptr);

    const QVector4D &color() const;
    void setColor(const QVector4D &newColor);

    float ambient() const;
    void setAmbient(float newAmbient);

    float diffuse() const;
    void setDiffuse(float newDiffuse);

    float specular() const;
    void setSpecular(float newSpecular);

private:
    QVector4D mColor;
    float mAmbient;
    float mDiffuse;
    float mSpecular;
};

#endif // LIGHT_H
