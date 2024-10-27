#pragma once

#include "Canavar/Engine/Util/Macros.h"

#include <memory>
#include <set>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QString>
#include <QVector3D>
#include <QtMath>

namespace Canavar::Engine
{
    class Node;
    using NodePtr = std::shared_ptr<Node>;
    using NodeWeakPtr = std::weak_ptr<Node>;

    class Node : public std::enable_shared_from_this<Node>
    {
      protected:
        Node();
        virtual ~Node() = default;

        DISABLE_COPY(Node);

      public:
        const QMatrix4x4& GetWorldTransformation();
        const QMatrix4x4& GetTransformation();

        const QQuaternion& GetWorldRotation();
        const QQuaternion& GetRotation() const { return mRotation; }

        const QVector3D& GetWorldPosition();
        const QVector3D& GetPosition() const { return mPosition; }
        const float GetPosition(int coord) const { return mPosition[coord]; }
        float* GetPosition(int coord) { return &mPosition[coord]; }

        const QVector3D& GetScale() const { return mScale; }
        const float GetScale(int coord) const { return mScale[coord]; }
        float* GetScale(int coord) { return &mScale[coord]; }

        const QMatrix3x3 GetNormalMatrix() const { return mNormalMatrix; }

        void SetWorldRotation(const QQuaternion& newRotation);
        void SetWorldPosition(const QVector3D& newPosition);
        void SetWorldPosition(float x, float y, float z);

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
        bool GetDirty() const { return mDirty; }
        void Update();
        void UpdateRotationFromEulerDegrees();

        float& GetYaw();
        float& GetPitch();
        float& GetRoll();

        NodePtr GetParent();

        virtual void SetParent(NodeWeakPtr pParentNode);
        virtual void AddChild(NodePtr pNode);
        virtual void RemoveChild(NodePtr pNode);

        const std::set<NodePtr>& GetChildren() const { return mChildren; }

      private:
        QMatrix4x4 mTransformation;
        QMatrix3x3 mNormalMatrix;
        QQuaternion mRotation;
        QVector3D mPosition{ QVector3D(0, 0, 0) };
        QVector3D mScale{ QVector3D(1, 1, 1) };

        // For caching
        QMatrix4x4 mWorldTransformation;
        QQuaternion mWorldRotation;
        QVector3D mWorldPosition;

        bool mDirty{ false };

        float mYaw{ 0 };
        float mPitch{ 0 };
        float mRoll{ 0 };

        bool mUpdateFromEulerAngles{ false };

        NodeWeakPtr mParent;
        std::set<NodePtr> mChildren;

        DEFINE_MEMBER(QString, NodeName);
    };

}
