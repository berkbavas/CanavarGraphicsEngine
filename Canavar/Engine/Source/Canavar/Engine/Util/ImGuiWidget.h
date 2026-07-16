#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"

#include <QtImGui.h>

namespace Canavar::Engine
{
    class Renderer;
    class Node;
    class Object;
    class Camera;
    class Light;
    class PointLight;
    class DirectionalLight;
    class TexturedModel;
    class NodeManager;
    class Sky;
    class Haze;
    class Terrain;

    class ImGuiWidget : public QObject, public EventReceiver
    {
        Q_OBJECT
      public:
        explicit ImGuiWidget(Renderer *pRenderer);

        void Initialize();
        void OnRenderOverlay(float Ifps);
        void DrawImGuiWidgets(float Ifps);

      private:
        // --- Top-level panels ---
        void DrawMenuBar();
        void DrawStats(float Ifps);
        void DrawNodeList();
        void DrawNodeProperties();

        // --- Per-type property drawers ---
        void DrawObjectTransform(Object *pObject);
        void DrawCameraProperties(Camera *pCamera);
        void DrawLightProperties(Light *pLight);
        void DrawPointLightProperties(PointLight *pLight);
        void DrawDirectionalLightProperties(DirectionalLight *pLight);
        void DrawTexturedModelProperties(TexturedModel *pModel);
        void DrawSkyProperties();
        void DrawHazeProperties();
        void DrawTerrainProperties();

        // --- Helpers ---
        void ValidateSelectedNode();

        QtImGui::RenderRef mRenderRef;
        Renderer *mRenderer{ nullptr };
        NodeManager *mNodeManager{ nullptr };
        CameraManager *mCameraManager{ nullptr };

        // Selection state
        Node *mSelectedNode{ nullptr };
        Node *mLastSelectedNode{ nullptr };

        // Text input buffers
        char mNodeNameBuffer[128]{};
    };

}