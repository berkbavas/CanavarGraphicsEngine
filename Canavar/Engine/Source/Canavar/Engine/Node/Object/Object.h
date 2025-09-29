#pragma once

#include "Canavar/Engine/Core/AABB.h"
#include "Canavar/Engine/Node/Node.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QString>
#include <QVector3D>
#include <QtMath>

namespace Canavar::Engine
{
    class Object;

    class Object : public Node
    {
      protected:
        Object() = default;
        ~Object() override = default;

        void ToJson(QJsonObject& Object) override;
        void FromJson(const QJsonObject& Object, const QSet<NodePtr>& Nodes) override;

      public:
        QMatrix4x4 GetWorldTransformation() const;
        const QMatrix4x4& GetTransformation() const { return mTransformation; }

        QQuaternion GetWorldRotation() const;
        const QQuaternion& GetRotation() const { return mRotation; }

        QVector3D GetWorldPosition() const;
        const QVector3D& GetPosition() const { return mPosition; }

        const QVector3D& GetScale() const { return mScale; }
        const QMatrix3x3& GetLocalNormalMatrix() const { return mNormalMatrix; }

        void SetWorldRotation(const QQuaternion& NewRotation);
        void SetWorldPosition(const QVector3D& NewPosition);
        void SetWorldPosition(float x, float y, float z);

        void SetTransformation(const QMatrix4x4& NewTransformation);
        void SetRotation(const QQuaternion& NewRotation);
        void SetPosition(const QVector3D& NewPosition);
        void SetPosition(float x, float y, float z);
        void SetScale(const QVector3D& NewScale);
        void SetScale(float x, float y, float z);
        void SetScale(float UniformScale);

        void RotateGlobal(const QVector3D& Axis, float Angle);
        void RotateLocal(const QVector3D& Axis, float Angle);
        void Translate(const QVector3D& Delta);

        void UpdateTransformation();

        float& GetYaw();
        float& GetPitch();
        float& GetRoll();

        void SetYaw(float Yaw);
        void SetPitch(float Pitch);
        void SetRoll(float Roll);

      private:
        QMatrix4x4 mTransformation;
        QMatrix3x3 mNormalMatrix;
        QQuaternion mRotation;
        QVector3D mPosition{ QVector3D(0, 0, 0) };
        QVector3D mScale{ QVector3D(1, 1, 1) };

        bool mTransformationDirty{ false };

        float mYaw{ 0 };
        float mPitch{ 0 };
        float mRoll{ 0 };

        DEFINE_MEMBER(bool, Visible, true);
        DEFINE_MEMBER(bool, Selectable, true);
        DEFINE_MEMBER(AABB, AABB, QVector3D(-1, -1, -1), QVector3D(1, 1, 1));
    };

    using ObjectPtr = std::shared_ptr<Object>;

}
