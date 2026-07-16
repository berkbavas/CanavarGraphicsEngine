#pragma once

#include "Canavar/Engine/Node/Node.h"
#include "Canavar/Engine/Object/AABB.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>

namespace Canavar::Engine
{
    class Object : public Node
    {
      public:
        Object() = default;
        virtual ~Object() = default;

        const QMatrix4x4& GetTransformation() const;
        const QQuaternion& GetRotation() const;
        const QVector3D& GetPosition() const;
        const QVector3D& GetScale() const;
        const QMatrix3x3& GetNormalMatrix() const;

        // World-space accessors (walks up the parent chain).
        QMatrix4x4 GetWorldTransformation() const;
        QVector3D GetWorldPosition() const;
        QQuaternion GetWorldRotation() const;
        QVector3D GetWorldScale() const;

        void SetTransformation(const QMatrix4x4& NewTransformation);
        void SetRotation(const QQuaternion& NewRotation);
        void SetPosition(const QVector3D& NewPosition);
        void SetPosition(float x, float y, float z);
        void SetScale(const QVector3D& NewScale);
        void SetScale(float x, float y, float z);
        void SetScale(float UniformScale);
        void Translate(const QVector3D& Delta);

      private:
        void UpdateTransformation();

        QMatrix4x4 mTransformation;
        QMatrix3x3 mNormalMatrix;
        QQuaternion mRotation;
        QVector3D mPosition{ QVector3D(0, 0, 0) };
        QVector3D mScale{ QVector3D(1, 1, 1) };

        DEFINE_MEMBER(bool, Visible, true);
        DEFINE_MEMBER(AABB, AABB, QVector3D(-1, -1, -1), QVector3D(1, 1, 1));
    };
}