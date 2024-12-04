#pragma once

#include "Canavar/Engine/Node/Node.h"
#include "Canavar/Engine/Node/Object/Scene/AABB.h"
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
    class Object;
    using ObjectPtr = std::shared_ptr<Object>;
    using ObjectWeakPtr = std::weak_ptr<Object>;

    using ObjectFactory = std::function<std::shared_ptr<Object>()>;

    class Object : public Node, public std::enable_shared_from_this<Object>
    {
      protected:
        Object() = default;
        ~Object() override = default;

        void ToJson(QJsonObject& object) override;
        void FromJson(const QJsonObject& object, const std::map<QString, NodePtr>& nodes) override;

      public:
        const QMatrix4x4& GetWorldTransformation();
        const QMatrix4x4& GetTransformation();

        const QQuaternion& GetWorldRotation();
        const QQuaternion& GetRotation() const { return mRotation; }

        const QVector3D& GetWorldPosition();
        const QVector3D& GetPosition() const { return mPosition; }

        const QVector3D& GetScale() const { return mScale; }
        const QMatrix3x3& GetLocalNormalMatrix() const { return mNormalMatrix; }

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

        void MakeTransformationDirty() { mTransformationDirty = true; }
        bool GetTransformationDirty() const { return mTransformationDirty; }
        void UpdateTransformation();

        float& GetYaw();
        float& GetPitch();
        float& GetRoll();

        void SetYaw(float yaw);
        void SetPitch(float pitch);
        void SetRoll(float roll);

        ObjectPtr GetParent() const;

        virtual void RemoveParent();
        virtual void SetParent(ObjectWeakPtr pParentNode);
        virtual void AddChild(ObjectPtr pNode);
        virtual void RemoveChild(ObjectPtr pNode);

        const std::set<ObjectPtr>& GetChildren() const { return mChildren; }

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

        bool mTransformationDirty{ false };

        float mYaw{ 0 };
        float mPitch{ 0 };
        float mRoll{ 0 };

        ObjectWeakPtr mParent;
        std::set<ObjectPtr> mChildren;

        DEFINE_MEMBER(bool, Visible, true);
        DEFINE_MEMBER(bool, Selectable, true);
        DEFINE_MEMBER(AABB, AABB, QVector3D(-1, -1, -1), QVector3D(1, 1, 1));

        template<class T>
        static ObjectFactory RegisterObjectFactory()
        {
            OBJECT_FACTORIES[T::NODE_TYPE] = []() { return std::dynamic_pointer_cast<Object>(std::make_shared<T>()); };
            return OBJECT_FACTORIES[T::NODE_TYPE];
        }

        inline static std::map<QString, ObjectFactory> OBJECT_FACTORIES{};
    };

}
