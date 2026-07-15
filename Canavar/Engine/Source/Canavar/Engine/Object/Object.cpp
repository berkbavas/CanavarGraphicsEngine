
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
    mTransformation.setToIdentity();
    mTransformation.scale(mScale);
    mTransformation.rotate(mRotation);
    mTransformation.setColumn(3, QVector4D(mPosition, 1.0f));

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
