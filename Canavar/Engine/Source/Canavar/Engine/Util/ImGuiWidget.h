#pragma once

#include "Canavar/Engine/Core/EventReceiver.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/Painter.h"
#include "Canavar/Engine/Manager/RenderingManager.h"
#include "Canavar/Engine/Node/NodeVisitor.h"
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

    class ImGuiWidget : public QObject, public EventReceiver, private NodeVisitor
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
        void GoToObject(Engine::Object *pObject);

      private:
        void DrawMenuBar();

        void DrawNodeTreeViewWidget();
        void DrawNodeParametersWidget();

        void DrawSun();
        void DrawSky();
        void DrawHaze();
        void DrawTerrain();

        void DrawObject(Object *pObject);
        void DrawModel(Model *pModel);
        void DrawText2D(Text2D *pText);
        void DrawText3D(Text3D *pText);
        void DrawCamera(PerspectiveCamera *pCamera);
        void DrawDirectionalLight(DirectionalLight *pLight);
        void DrawPointLight(PointLight *pLight);
        void DrawNozzleEffect(NozzleEffect *pEffect);
        void DrawLightningStrike(LightningStrikeBase *pLightning);

        void DrawCreateObjectWidget();
        void DrawCreateModelWidget();
        void DrawRenderSettings();
        void DrawPainterSettings();
        void DrawNodeInfo();
        void DrawStats();

        void DrawWorldPositionsWidget();

        QVector3D GetWorldPositionForCreatedObject() const;

        std::optional<std::string> InputText(const std::string &label, const std::string &text);

        void RemoveNode(Node *pNode);
        void SetSelectedNode(NodePtr pNode);
        void SetSelectedMesh(NodePtr pNode, uint32_t MeshId);
        void ProcessMouseAction(int x, int y);

        void Visit(Sun &) override { DrawSun(); }
        void Visit(Sky &) override { DrawSky(); }
        void Visit(Haze &) override { DrawHaze(); }
        void Visit(Terrain &) override { DrawTerrain(); }
        void Visit(Text2D &text2D) override { DrawText2D(&text2D); }
        void Visit(Object &object) override { DrawObject(&object); }

        void Visit(Model &model) override
        {
            DrawObject(&model);
            DrawModel(&model);
        }

        void Visit(Text3D &text3D) override
        {
            DrawObject(&text3D);
            DrawText3D(&text3D);
        }

        void Visit(PerspectiveCamera &camera) override
        {
            DrawObject(&camera);
            DrawCamera(&camera);
        }

        void Visit(DirectionalLight &light) override
        {
            DrawObject(&light);
            DrawDirectionalLight(&light);
        }

        void Visit(PointLight &light) override
        {
            DrawObject(&light);
            DrawPointLight(&light);
        }

        void Visit(NozzleEffect &effect) override
        {
            DrawObject(&effect);
            DrawNozzleEffect(&effect);
        }
        
        void Visit(LightningStrikeBase &strike) override
        {
            DrawObject(&strike);
            DrawLightningStrike(&strike);
        }

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