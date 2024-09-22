#include "AABB.h"

Canavar::Engine::AABB::AABB() {}

QMatrix4x4 Canavar::Engine::AABB::GetTransformation() const
{
    QMatrix4x4 result;
    result.translate((mMin + mMax) / 2.0f);
    result.scale(mMax.x() - mMin.x(), mMax.y() - mMin.y(), mMax.z() - mMin.z());
    return result;
}

Canavar::Engine::AABB Canavar::Engine::AABB::Transform(const QMatrix4x4 &transformation) const
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
        vertices[i] = (transformation * QVector4D(vertices[i], 1)).toVector3D();

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

    AABB aabb;
    aabb.SetMin(min);
    aabb.SetMax(max);
    return aabb;
}

void Canavar::Engine::AABB::ToJson(QJsonObject &object)
{
    QJsonObject aabb;

    QJsonObject min;
    min.insert("x", mMin.x());
    min.insert("y", mMin.y());
    min.insert("z", mMin.z());
    aabb.insert("min", min);

    QJsonObject max;
    max.insert("x", mMax.x());
    max.insert("y", mMax.y());
    max.insert("z", mMax.z());
    aabb.insert("max", max);

    object.insert("aabb", aabb);
}

void Canavar::Engine::AABB::FromJson(const QJsonObject &object)
{
    // Min
    {
        float x = object["min"]["x"].toDouble();
        float y = object["min"]["y"].toDouble();
        float z = object["min"]["z"].toDouble();

        mMin = QVector3D(x, y, z);
    }

    // Max
    {
        float x = object["max"]["x"].toDouble();
        float y = object["max"]["y"].toDouble();
        float z = object["max"]["z"].toDouble();

        mMax = QVector3D(x, y, z);
    }
}
