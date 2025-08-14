#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/RenderingManager/RenderingManager.h"
#include "Canavar/Engine/Manager/Painter.h"
#include "Canavar/Engine/Node/Object/Effect/NozzleEffect/NozzleEffect.h"
#include "Canavar/Engine/Node/Object/Light/DirectionalLight.h"
#include "Canavar/Engine/Node/Object/Light/PointLight.h"
#include "Canavar/Engine/Node/Object/LightningStrike/LightningStrikeGenerator.h"
#include "Canavar/Engine/Node/Object/Model/Model.h"
#include "Canavar/Engine/Node/Object/Text/Text2D.h"
#include "Canavar/Engine/Node/Object/Text/Text3D.h"
#include "Canavar/Engine/Util/Macros.h"

namespace Canavar::Engine
{
    class ImGuiWidget : public QObject, public EventReceiver
    {
        Q_OBJECT
      public:
        explicit ImGuiWidget(QObject *pParent);

        void PostInitialize() override;
        void DrawImGui(float ifps) override;

        // EventReceiver overrides
        bool KeyPressed(QKeyEvent *) override;
        bool KeyReleased(QKeyEvent *) override;
        bool MousePressed(QMouseEvent *) override;
        bool MouseReleased(QMouseEvent *) override;
        bool MouseMoved(QMouseEvent *) override;
        bool WheelMoved(QWheelEvent *) override;

      signals:
        void GoToObject(Engine::ObjectPtr pObject);

      private:
        void DrawMenuBar();

        void DrawNodeTreeViewWidget();
        void DrawNodeParametersWidget();

        void DrawSun();
        void DrawSky();
        void DrawHaze();
        void DrawTerrain();

        void DrawObject(Engine::ObjectPtr pObject);
        void DrawModel(Engine::ModelPtr pModel);
        void DrawText2D(Engine::Text2DPtr pText);
        void DrawText3D(Engine::Text3DPtr pText);
        void DrawCamera(Engine::PerspectiveCameraPtr pCamera);
        void DrawDirectionalLight(Engine::DirectionalLightPtr pLight);
        void DrawPointLight(Engine::PointLightPtr pLight);
        void DrawNozzleEffect(Engine::NozzleEffectPtr pEffect);
        void DrawLightningStrike(Engine::LightningStrikeBasePtr pLightning);

        void DrawCreateObjectWidget();
        void DrawCreateModelWidget();
        void DrawRenderSettings();
        void DrawPainterSettings();
        void DrawNodeInfo();
        void DrawStats();

        void DrawWorldPositionsWidget();

        QVector3D GetWorldPositionForCreatedObject() const;

        std::optional<std::string> InputText(const std::string &label, const std::string &text);

        void RemoveNode(NodePtr pNode);

        void SetSelectedNode(NodePtr pNode);
        void SetSelectedMesh(NodePtr pNode, uint32_t MeshId);
        void ProcessMouseAction(int x, int y);

        DEFINE_MEMBER_PTR(Engine::NodeManager, NodeManager);
        DEFINE_MEMBER_PTR(Engine::CameraManager, CameraManager);
        DEFINE_MEMBER_PTR(Engine::RenderingManager, RenderingManager);
        DEFINE_MEMBER_PTR(Engine::Painter, Painter);

        Engine::NodePtr mSelectedNode{ nullptr };
        Engine::MeshPtr mSelectedMesh{ nullptr };

        Engine::DirectionalLightPtr mSun;
        Engine::SkyPtr mSky;
        Engine::HazePtr mHaze;
        Engine::TerrainPtr mTerrain;

        QVector3D mFragmentLocalPosition;
        QVector3D mFragmentWorldPosition;
        Engine::NodeInfo mNodeInfo;

        QVector<QVector3D> mSavedWorldPositions;
        int mSelectedWorldPositionIndex{ -1 };

        std::string mSelectedSceneName = "-";
        std::string mSelectedObjectName = "-";
    };

}