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
    QVector<QVector3D> Vertices;
    Vertices << mMin;

    Vertices << QVector3D(mMax.x(), mMin.y(), mMin.z());
    Vertices << QVector3D(mMin.x(), mMax.y(), mMin.z());
    Vertices << QVector3D(mMin.x(), mMin.y(), mMax.z());

    Vertices << mMax;
    Vertices << QVector3D(mMin.x(), mMax.y(), mMax.z());
    Vertices << QVector3D(mMax.x(), mMin.y(), mMax.z());
    Vertices << QVector3D(mMax.x(), mMax.y(), mMin.z());

    for (int i = 0; i < Vertices.size(); ++i)
    {
        Vertices[i] = (Transformation * QVector4D(Vertices[i], 1)).toVector3D();
    }

    float Inf = std::numeric_limits<float>::infinity();
    QVector3D Min(Inf, Inf, Inf);
    QVector3D Max(-Inf, -Inf, -Inf);

    for (int i = 0; i < Vertices.size(); ++i)
    {
        if (Min[0] > Vertices[i].x())
            Min[0] = Vertices[i].x();

        if (Min[1] > Vertices[i].y())
            Min[1] = Vertices[i].y();

        if (Min[2] > Vertices[i].z())
            Min[2] = Vertices[i].z();

        if (Max[0] < Vertices[i].x())
            Max[0] = Vertices[i].x();

        if (Max[1] < Vertices[i].y())
            Max[1] = Vertices[i].y();

        if (Max[2] < Vertices[i].z())
            Max[2] = Vertices[i].z();
    }

    return AABB(Min, Max);
}
