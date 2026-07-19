
#include "Object.h"

void Canavar::Engine::Object::SetRotation(const QQuaternion& NewRotation)
{
    mRotation = NewRotation;
    UpdateTransformation();
}

void Canavar::Engine::Object::SetPosition(const QVector3D& NewPosition)
{
    mPosition = NewPosition;
    UpdateTransformation();
}

void Canavar::Engine::Object::SetScale(const QVector3D& NewScale)
{
    mScale = NewScale;
    UpdateTransformation();
}

const QMatrix4x4& Canavar::Engine::Object::GetTransformation() const
{
    return mTransformation;
}

void Canavar::Engine::Object::SetTransformation(const QMatrix4x4& NewTransformation)
{
    // Decompose the matrix into position, rotation, and scale.
    mTransformation = NewTransformation;
    mPosition = mTransformation.column(3).toVector3D();
    mScale = QVector3D(mTransformation.column(0).length(), mTransformation.column(1).length(), mTransformation.column(2).length());
    QMatrix4x4 RotationMatrix = mTransformation;
    RotationMatrix.setColumn(0, RotationMatrix.column(0) / mScale.x());
    RotationMatrix.setColumn(1, RotationMatrix.column(1) / mScale.y());
    RotationMatrix.setColumn(2, RotationMatrix.column(2) / mScale.z());
    mRotation = QQuaternion::fromRotationMatrix(RotationMatrix.toGenericMatrix<3, 3>());
    mNormalMatrix = mTransformation.normalMatrix();
}

void Canavar::Engine::Object::SetPosition(float x, float y, float z)
{
    SetPosition(QVector3D(x, y, z));
}

void Canavar::Engine::Object::SetScale(float x, float y, float z)
{
    SetScale(QVector3D(x, y, z));
}

void Canavar::Engine::Object::SetScale(float UniformScale)
{
    SetScale(UniformScale, UniformScale, UniformScale);
}

void Canavar::Engine::Object::Translate(const QVector3D& Delta)
{
    SetPosition(GetPosition() + Delta);
}

void Canavar::Engine::Object::UpdateTransformation()
{
    // Rebuild the transformation matrix from position, rotation, and scale.
    // Note: The order of transformations is important.
    // We scale first, then rotate, then translate.
    // But reverse order of operations is applied when multiplying matrices.
    // For example, if you want to scale an object, then rotate it, and finally translate it,
    // you would multiply the matrices in the order: Translation * Rotation * Scale.
    // Translation is applied last, so it should be the first matrix in the multiplication order.

    mTransformation.setToIdentity();
    mTransformation.translate(mPosition);
    mTransformation.rotate(mRotation);
    mTransformation.scale(mScale);
    mNormalMatrix = mTransformation.normalMatrix();
}

const QQuaternion& Canavar::Engine::Object::GetRotation() const
{
    return mRotation;
}

const QVector3D& Canavar::Engine::Object::GetPosition() const
{
    return mPosition;
}

const QVector3D& Canavar::Engine::Object::GetScale() const
{
    return mScale;
}

const QMatrix3x3& Canavar::Engine::Object::GetNormalMatrix() const
{
    return mNormalMatrix;
}

float Canavar::Engine::Object::GetBoundingSphereRadius() const
{
    // Calculate the bounding sphere radius based on the AABB and scale.
    const AABB& LocalAABB = GetAABB();
    const QVector3D& Scale = GetScale();

    // Calculate the half extents of the AABB in local space
    QVector3D HalfExtents = (LocalAABB.GetMax() - LocalAABB.GetMin()) * 0.5f;

    // Scale the half extents by the object's scale
    HalfExtents.setX(HalfExtents.x() * Scale.x());
    HalfExtents.setY(HalfExtents.y() * Scale.y());
    HalfExtents.setZ(HalfExtents.z() * Scale.z());

    // The bounding sphere radius is the length of the scaled half extents vector
    return HalfExtents.length();
}

// ─────────────────────────────────────────────────────────────────────────────
// World-space accessors
// ─────────────────────────────────────────────────────────────────────────────

QMatrix4x4 Canavar::Engine::Object::GetWorldTransformation() const
{
    if (const auto* pParent = dynamic_cast<const Object*>(GetParent()))
    {
        return pParent->GetWorldTransformation() * mTransformation;
    }
    return mTransformation;
}

QVector3D Canavar::Engine::Object::GetWorldPosition() const
{
    return GetWorldTransformation().column(3).toVector3D();
}

QQuaternion Canavar::Engine::Object::GetWorldRotation() const
{
    const QMatrix4x4 World = GetWorldTransformation();
    const QVector3D Scale(World.column(0).toVector3D().length(), World.column(1).toVector3D().length(), World.column(2).toVector3D().length());

    QMatrix4x4 RotMatrix = World;
    RotMatrix.setColumn(0, RotMatrix.column(0) / Scale.x());
    RotMatrix.setColumn(1, RotMatrix.column(1) / Scale.y());
    RotMatrix.setColumn(2, RotMatrix.column(2) / Scale.z());
    return QQuaternion::fromRotationMatrix(RotMatrix.toGenericMatrix<3, 3>());
}

QVector3D Canavar::Engine::Object::GetWorldScale() const
{
    const QMatrix4x4 World = GetWorldTransformation();
    return QVector3D(World.column(0).toVector3D().length(), World.column(1).toVector3D().length(), World.column(2).toVector3D().length());
}
