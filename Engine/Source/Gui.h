#pragma once

#include "FirecrackerEffect.h"
#include "FreeCamera.h"
#include "LineStrip.h"
#include "Mesh.h"
#include "Model.h"
#include "Node.h"
#include "NozzleEffect.h"
#include "PerspectiveCamera.h"
#include "Sky.h"
#include "Sun.h"
#include "Terrain.h"
#include "LightningStrikeGenerator.h"
#include "LightningStrikeAttractor.h"
#include "LightningStrikeSpherical.h"

#include <imgui.h>
#include <QtImGui.h>

namespace Canavar {
    namespace Engine {
        class Gui : public QObject
        {
            Q_OBJECT

        public:
            explicit Gui(QObject* parent = nullptr);

            void Draw();

            void Draw(Canavar::Engine::Node* node);
            void Draw(Canavar::Engine::Model* node);
            void Draw(Canavar::Engine::Sky* node);
            void Draw(Canavar::Engine::Sun* node);
            void Draw(Canavar::Engine::PerspectiveCamera* node);
            void Draw(Canavar::Engine::FreeCamera* node);
            void Draw(Canavar::Engine::Terrain* node);
            void Draw(Canavar::Engine::Light* node);
            void Draw(Canavar::Engine::PointLight* node);
            void Draw(Canavar::Engine::Haze* node);
            void Draw(Canavar::Engine::NozzleEffect* node);
            void Draw(Canavar::Engine::FirecrackerEffect* node);
            void Draw(Canavar::Engine::LightningStrikeGenerator* node);
            void Draw(Canavar::Engine::LightningStrikeAttractor* node);
            void Draw(Canavar::Engine::LightningStrikeSpherical* node);

            Canavar::Engine::Node* GetSelectedNode() const;
            void SetSelectedNode(Canavar::Engine::Node* newSelectedNode);

            Canavar::Engine::Mesh* GetSelectedMesh() const;
            void SetSelectedMesh(Canavar::Engine::Mesh* newSelectedMesh);

            void MousePressed(QMouseEvent* event);
            void MouseMoved(QMouseEvent* event);
            void MouseReleased(QMouseEvent* event);
            void KeyPressed(QKeyEvent* event);

            void Resize(int w, int h);

            int GetSelectedVertexIndex() const;
            void SetSelectedVertexIndex(int newSelectedVertexIndex);

        signals:
            void ShowFileDialog();

        private:
            NodeManager* mNodeManager;
            Node* mSelectedNode;
            Model* mSelectedModel; // Casted from mSelectedNode
            Mesh* mSelectedMesh;
            int mSelectedVertexIndex;
            bool mDrawAllBBs;

            bool mNodeSelectionEnabled;
            bool mMeshSelectionEnabled;
            bool mVertexSelectionEnabled;

            // Create a Node Window
            QStringList mCreatableNodeNames;
            QString mSelectedNodeName;
            QString mSelectedModelName;

            // Move
            Mouse mMouse;
            int mWidth;
            int mHeight;

            bool mMoveEnabled;
            bool mLockNode;
            bool mLockAxisX;
            bool mLockAxisY;
            bool mLockAxisZ;

            // Intersection Debug
            LineStrip* mLineStrip;
            QVector3D mRayDirection;
            Model* mCube;
        };
    } // namespace Engine
} // namespace Canavar
