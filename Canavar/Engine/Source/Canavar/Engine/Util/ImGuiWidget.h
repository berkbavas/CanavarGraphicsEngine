#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Core/EventThief.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/PostProcessEffect/AcesEffect.h"
#include "Canavar/Engine/PostProcessEffect/ChromaticAberrationEffect.h"
#include "Canavar/Engine/PostProcessEffect/CinematicEffect.h"
#include "Canavar/Engine/PostProcessEffect/ColorGradingEffect.h"
#include "Canavar/Engine/PostProcessEffect/DepthOfFieldEffect.h"
#include "Canavar/Engine/PostProcessEffect/FxaaEffect.h"
#include "Canavar/Engine/PostProcessEffect/LensDistortionEffect.h"
#include "Canavar/Engine/PostProcessEffect/SharpenEffect.h"

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
    class PrimitiveModel;
    class NodeManager;
    class Sky;
    class Haze;
    class Terrain;

    class ImGuiWidget : public QObject, public EventReceiver, public EventThief
    {
        Q_OBJECT
      public:
        explicit ImGuiWidget(Renderer *pRenderer);

        bool WantsKeyboardCapture() const override;
        bool WantsMouseCapture() const override;

      signals:
        void CanDrawImGuiWidgets(float Ifps);

      private:
        // Private slots for handling signals from the renderer"
        void Initialize();
        void OnPostRender(float Ifps);
        void DrawImGuiWidgets(float Ifps);

        // --- Top-level panels ---
        void DrawMenuBar();
        void DrawStats(float Ifps);
        void DrawNodeList();
        void DrawNodeProperties();

        // --- Per-type property drawers ---
        void DrawObjectTransform(Object *pObject);
        void DrawHierarchyProperties(Node *pNode);
        void DrawCameraProperties(Camera *pCamera);
        void DrawLightProperties(Light *pLight);
        void DrawPointLightProperties(PointLight *pLight);
        void DrawDirectionalLightProperties(DirectionalLight *pLight);
        void DrawTexturedModelProperties(TexturedModel *pModel);
        void DrawPrimitiveModelProperties(PrimitiveModel *pModel);
        void DrawSkyProperties();
        void DrawHazeProperties();
        void DrawTerrainProperties();
        void DrawPostProcessPanel();
        void DrawRendererProperties();
        void EnterGizmoIfApplicable();
        void ExitGizmoIfApplicable();

        // --- Helpers ---
        void ValidateSelectedNode();
        void DrawNodeTree(Node *pNode);

        void SetSelectedNode(Node *pNode);
        void UpdateNameBuffer();

        QtImGui::RenderRef mRenderRef;
        Renderer *mRenderer{ nullptr };
        NodeManager *mNodeManager{ nullptr };
        CameraManager *mCameraManager{ nullptr };

        // Selection state
        Node *mSelectedNode{ nullptr };

        // Text input buffers
        char mNodeNameBuffer[128]{};

        bool mGizmoEnabled{ false };
    };

    using ImGuiWidgetPtr = std::unique_ptr<ImGuiWidget>;
}