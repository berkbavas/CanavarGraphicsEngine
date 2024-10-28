#pragma once

#include <Canavar/Engine/Manager/NodeManager.h>
#include <Canavar/Engine/Manager/RenderingManager/RenderingManager.h>
#include <Canavar/Engine/Node/Light/DirectionalLight.h>
#include <Canavar/Engine/Util/Macros.h>

namespace Canavar::Editor
{
    class ImGuiWidget : public QObject
    {
        Q_OBJECT
      public:
        void Initialize();
        void DrawWidget();

        bool KeyPressed(QKeyEvent *);
        bool KeyReleased(QKeyEvent *);
        bool MousePressed(QMouseEvent *);
        bool MouseReleased(QMouseEvent *);
        bool MouseMoved(QMouseEvent *);

      signals:
        void GoToNode(Engine::NodePtr pNode);

      private:
        void DrawRenderSettings();
        void DrawNodes();
        void DrawNode(Engine::NodePtr pNode);

        void DrawSun();
        void DrawSky();
        void DrawTerrain();
        void DrawHaze();

        Engine::NodePtr mSelectedNode{ nullptr };

        Engine::DirectionalLightPtr mSun;
        Engine::SkyPtr mSky;
        Engine::TerrainPtr mTerrain;
        Engine::HazePtr mHaze;

        QVector3D mFragmentLocalPosition;
        QVector3D mFragmentWorldPosition;

        DEFINE_MEMBER_PTR(Engine::NodeManager, NodeManager);
        DEFINE_MEMBER_PTR(Engine::RenderingManager, RenderingManager);
    };

}