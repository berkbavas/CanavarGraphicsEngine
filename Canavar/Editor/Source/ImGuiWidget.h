#pragma once

#include <Canavar/Engine/Manager/NodeManager.h>
#include <Canavar/Engine/Manager/RenderingManager.h>
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

        DEFINE_MEMBER_PTR(Engine::NodeManager, NodeManager);
        DEFINE_MEMBER_PTR(Engine::RenderingManager, RenderingManager);
    };

}