#pragma once

#include <Canavar/Engine/Manager/NodeManager.h>
#include <Canavar/Engine/Node/Light/DirectionalLight.h>
#include <Canavar/Engine/Util/Macros.h>

namespace Canavar::Editor
{
    class ImGuiWidget
    {
      public:
        void Initialize();
        void DrawWidget();

      private:
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
    };

}