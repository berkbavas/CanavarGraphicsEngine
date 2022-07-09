#include "Model.h"

Model::Model(QObject *parent)
    : Node{parent}
{}

Model::~Model() {}

Model::Type Model::type() const
{
    return mType;
}

void Model::setType(Type newType)
{
    mType = newType;
}

Material &Model::material()
{
    return mMaterial;
}

void Model::setMaterial(const Material &newMaterial)
{
    mMaterial = newMaterial;
}

const QVector<Model::Type> Model::ALL_MODEL_TYPES = {
    Camera,
    Capsule,
    Cone,
    Cube,
    Cylinder,
    Dome,
    Plane,
    Pyramid,
    Sphere,
    Suzanne,
    Tetrahedron,
    Torus,
    TorusKnot,
};
