#include "AABB.h"

Canavar::Engine::AABB::AABB(const QVector3D& Min, const QVector3D& Max)
    : mMin(Min)
    , mMax(Max)
{
    mCenter = (mMin + mMax) / 2.0f;
    mTransformation.translate(mCenter);
    mTransformation.scale(mMax.x() - mMin.x(), mMax.y() - mMin.y(), mMax.z() - mMin.z());
}

Canavar::Engine::AABB Canavar::Engine::AABB::Transform(const QMatrix4x4& Transformation) const
{
    QVector<QVector3D> vertices;
    vertices << mMin;

    vertices << QVector3D(mMax.x(), mMin.y(), mMin.z());
    vertices << QVector3D(mMin.x(), mMax.y(), mMin.z());
    vertices << QVector3D(mMin.x(), mMin.y(), mMax.z());

    vertices << mMax;
    vertices << QVector3D(mMin.x(), mMax.y(), mMax.z());
    vertices << QVector3D(mMax.x(), mMin.y(), mMax.z());
    vertices << QVector3D(mMax.x(), mMax.y(), mMin.z());

    for (int i = 0; i < vertices.size(); ++i)
    {
        vertices[i] = (Transformation * QVector4D(vertices[i], 1)).toVector3D();
    }

    float inf = std::numeric_limits<float>::infinity();
    QVector3D min(inf, inf, inf);
    QVector3D max(-inf, -inf, -inf);

    for (int i = 0; i < vertices.size(); ++i)
    {
        if (min[0] > vertices[i].x())
            min[0] = vertices[i].x();

        if (min[1] > vertices[i].y())
            min[1] = vertices[i].y();

        if (min[2] > vertices[i].z())
            min[2] = vertices[i].z();

        if (max[0] < vertices[i].x())
            max[0] = vertices[i].x();

        if (max[1] < vertices[i].y())
            max[1] = vertices[i].y();

        if (max[2] < vertices[i].z())
            max[2] = vertices[i].z();
    }

    return AABB(min, max);
}
