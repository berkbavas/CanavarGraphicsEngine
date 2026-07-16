#include "Primitives.h"

Canavar::Engine::Line::Line()
    : OutlinePrimitive(PrimitiveType::Line)
{
    SetNodeName("Line");
}

void Canavar::Engine::Line::SetPoints(const QVector3D &StartPoint, const QVector3D &EndPoint)
{
    const auto Direction = EndPoint - StartPoint;
    const auto Length = Direction.length();

    QMatrix4x4 Transformation;
    Transformation.setToIdentity();
    if (Length > 0.0f)
    {
        const auto NormalizedDirection = Direction / Length;
        const auto Rotation = QQuaternion::rotationTo(QVector3D(0.0f, 0.0f, 1.0f), NormalizedDirection);
        Transformation.rotate(Rotation);
    }
    Transformation.translate(StartPoint);
    Transformation.scale(1.0f, 1.0f, Length);
    SetTransformation(Transformation);
}

Canavar::Engine::Circle::Circle()
    : OutlinePrimitive(PrimitiveType::Circle)
{
    SetNodeName("Circle");
}

Canavar::Engine::Disk::Disk()
    : PrimitiveModel(PrimitiveType::Disk)
{
    SetNodeName("Disk");
}

Canavar::Engine::Plane::Plane()
    : PrimitiveModel(PrimitiveType::Plane)
{
    SetNodeName("Plane");
}

Canavar::Engine::Sphere::Sphere()
    : PrimitiveModel(PrimitiveType::Sphere)
{
    SetNodeName("Sphere");
}