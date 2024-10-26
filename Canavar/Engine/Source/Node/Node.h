#pragma once

#include "Util/Macros.h"

#include <memory>
#include <string>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>
#include <QtMath>

namespace Canavar::Engine
{
    class Node
    {
      protected:
        Node() = default;
        virtual ~Node() = default;

        DISABLE_COPY(Node);

      public:
        const QMatrix4x4& GetTransformation() const { return mTransformation; }
        const QQuaternion& GetRotation() const { return mRotation; }

        const QVector3D& GetPosition() const { return mPosition; }
        const float GetPosition(int coord) const { return mPosition[coord]; }
        float* GetPosition(int coord) { return &mPosition[coord]; }

        const QVector3D& GetScale() const { return mScale; }
        const float GetScale(int coord) const { return mScale[coord]; }
        float* GetScale(int coord) { return &mScale[coord]; }

        const QMatrix3x3 GetNormalMatrix() const { return mNormalMatrix; }

        void SetTransformation(const QMatrix4x4& newTransformation);
        void SetRotation(const QQuaternion& newRotation);
        void SetPosition(const QVector3D& newPosition);
        void SetPosition(float x, float y, float z);
        void SetScale(const QVector3D& newScale);
        void SetScale(float x, float y, float z);

        void RotateGlobal(const QVector3D& axis, float angle);
        void RotateLocal(const QVector3D& axis, float angle);
        void Translate(const QVector3D& delta);

        void MakeDirty() { mDirty = true; }
        void MakeClean() { mDirty = false; }
        bool GetDirty() const { return mDirty; }

        void Update();

      private:
        virtual void UpdateTransformation();

        QMatrix4x4 mTransformation;
        QMatrix3x3 mNormalMatrix;
        QQuaternion mRotation;
        QVector3D mPosition{ QVector3D(0, 0, 0) };
        QVector3D mScale{ QVector3D(1, 1, 1) };

        bool mDirty{ false };

        DEFINE_MEMBER(std::string, NodeName);
    };

    using NodePtr = std::shared_ptr<Node>;
}
