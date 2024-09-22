#pragma once

#include "Manager.h"
#include "Node.h"

#include <QObject>

namespace Canavar {
    namespace Engine {
        class CameraManager;
        class LightManager;
        class ModelDataManager;

        class NodeManager : public Manager
        {
            Q_OBJECT
        private:
            NodeManager();

        public:
            static NodeManager* Instance();

            bool Init() override;
            void PostInit() override;

            Node* CreateNode(Node::NodeType type, const QString& name = QString());
            Model* CreateModel(const QString& modelName, const QString& name = QString());

            void RemoveNode(Node* node);

            Node* GetNodeByID(int ID);
            Node* GetNodeByUUID(const QString& uuid);
            Node* GetNodeByName(const QString& name);
            Node* GetNodeByScreenPosition(int x, int y);

            const QList<Node*>& GetNodes() const;

            void ToJson(QJsonObject& object);

        signals:
            void NodeCreated(Canavar::Engine::Node* node);

        private:
            void AssignName(Node* node, const QString& name);

        private:
            QList<Node*> mNodes;
            int mNumberOfNodes;

            CameraManager* mCameraManager;
            LightManager* mLightManager;
            ModelDataManager* mModelDataManager;

            QMap<Node::NodeType, QString> mTypeToName;
        };
    } // namespace Engine
} // namespace Canavar
