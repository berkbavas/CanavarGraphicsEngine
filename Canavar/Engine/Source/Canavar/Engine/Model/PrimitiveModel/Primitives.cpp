#include "Primitives.h"

Canavar::Engine::Line::Line()
    : OutlinePrimitive(PrimitiveType::Line)
{
    SetNodeName("Line");
}

void Canavar::Engine::Line::SetPoints(const QVector3D &StartPoint, const QVector3D &EndPoint)
{
    // First scale, then rotate, then translate.
    // This is because we want to scale the line along its local Z axis,
    // then rotate it to align with the desired direction,
    // and finally translate it to the starting point.

    // Remember that the order of transformations is important.
    // In this case, we want to apply the transformations in the following order:
    // Translate * Rotate * Scale
    // However, when we multiply matrices, the last transformation is applied first.
    // Because QMatrix::scale multiplies the current matrix from the right, the scale is applied first.

    const auto Direction = EndPoint - StartPoint;
    const auto Length = Direction.length();

    QMatrix4x4 Transformation;
    Transformation.setToIdentity();
    Transformation.translate(StartPoint);

    if (Length > 0.0f)
    {
        const auto NormalizedDirection = Direction / Length;
        const auto Rotation = QQuaternion::rotationTo(QVector3D(0.0f, 0.0f, 1.0f), NormalizedDirection);
        Transformation.rotate(Rotation);
    }

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