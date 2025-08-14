#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/Painter.h"
#include "Canavar/Engine/Manager/RenderingManager.h"
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
    class RenderingContext;

    class ImGuiWidget : public QObject, public EventReceiver
    {
        Q_OBJECT
      public:
        explicit ImGuiWidget(RenderingContext *pRenderingContext, QObject *pParent);

        void PostInitialize();
        void DrawImGui(float ifps);

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

        void DrawObject(ObjectPtr pObject);
        void DrawModel(ModelPtr pModel);
        void DrawText2D(Text2DPtr pText);
        void DrawText3D(Text3DPtr pText);
        void DrawCamera(PerspectiveCameraPtr pCamera);
        void DrawDirectionalLight(DirectionalLightPtr pLight);
        void DrawPointLight(PointLightPtr pLight);
        void DrawNozzleEffect(NozzleEffectPtr pEffect);
        void DrawLightningStrike(LightningStrikeBasePtr pLightning);

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

        RenderingContext *mRenderingContext{ nullptr };
        NodeManager *mNodeManager{ nullptr };
        CameraManager *mCameraManager{ nullptr };
        RenderingManager *mRenderingManager{ nullptr };
        Painter *mPainter{ nullptr };
        ShadowMappingRenderer *mShadowMappingRenderer{ nullptr };
        BoundingBoxRenderer *mBoundingBoxRenderer{ nullptr };

        NodePtr mSelectedNode{ nullptr };
        MeshPtr mSelectedMesh{ nullptr };

        DirectionalLightPtr mSun;
        SkyPtr mSky;
        HazePtr mHaze;
        TerrainPtr mTerrain;

        QVector3D mFragmentLocalPosition;
        QVector3D mFragmentWorldPosition;
        NodeInfo mNodeInfo;

        QVector<QVector3D> mSavedWorldPositions;
        int mSelectedWorldPositionIndex{ -1 };

        std::string mSelectedSceneName = "-";
        std::string mSelectedObjectName = "-";
    };

}