#pragma once

#include "AABB.h"
#include "Common.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QMatrix4x4>
#include <QObject>
#include <QQuaternion>
#include <QVector3D>

namespace Canavar {
    namespace Engine {
        class Node : public QObject
        {
            Q_OBJECT
        protected:
            friend class NodeManager;

            Node();
            virtual ~Node();

            virtual void ToJson(QJsonObject& object);
            virtual void FromJson(const QJsonObject& object);

        public:
            enum class NodeType { //
                DummyNode,
                Model,
                FreeCamera,
                DummyCamera,
                Sun,
                Sky,
                Haze,
                Terrain,
                PointLight,
                NozzleEffect,
                FirecrackerEffect,
                PersecutorCamera,
                LightningStrikeGenerator,
                LightningStrikeAttractor,
                LightningStrikeSpherical,
            };

            const QMatrix4x4 WorldTransformation() const;
            void SetWorldTransformation(const QMatrix4x4& newTransformation);

            const QMatrix4x4& Transformation() const;
            void SetTransformation(const QMatrix4x4& newTransformation);

            QQuaternion WorldRotation() const;
            void SetWorldRotation(const QQuaternion& newWorldRotation);

            const QQuaternion& Rotation() const;
            void SetRotation(const QQuaternion& newRotation);

            QVector3D WorldPosition() const;
            void SetWorldPosition(const QVector3D& newWorldPosition);

            const QVector3D& Position() const;
            void SetPosition(const QVector3D& newPosition);

            const QVector3D& Scale() const;
            void SetScale(const QVector3D& newScale);

            Node* GetParent() const;
            void SetParent(Node* newParent);

            bool IsChildOf(Node* node);
            bool IsParentOf(Node* node);

            void AddChild(Node* node);
            void RemoveChild(Node* node);

            Node* FindChildByNameRecursive(const QString& name);

            const QList<Node*>& GetChildren() const;

        private:
            virtual void UpdateTransformation();

        protected:
            QMatrix4x4 mTransformation;
            QQuaternion mRotation;
            QVector3D mPosition;
            QVector3D mScale;

            Node* mParent;
            QList<Node*> mChildren;

            DEFINE_MEMBER(bool, Visible);
            DEFINE_MEMBER(QString, Name);
            DEFINE_MEMBER(AABB, AABB);
            DEFINE_MEMBER(bool, Selectable);

            DEFINE_MEMBER_CONST(QString, UUID);
            DEFINE_MEMBER_CONST(int, ID);
            DEFINE_MEMBER_CONST(NodeType, Type);

            DEFINE_MEMBER(bool, ExcludeFromExport);
        };
    } // namespace Engine
} // namespace Canavar
