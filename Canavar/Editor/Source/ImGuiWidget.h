#pragma once

#include <Canavar/Engine/Manager/NodeManager.h>
#include <Canavar/Engine/Manager/RenderingManager/RenderingManager.h>
#include <Canavar/Engine/Node/Object/Light/DirectionalLight.h>
#include <Canavar/Engine/Util/Macros.h>

namespace Canavar::Editor
{
    class ImGuiWidget : public QObject
    {
        Q_OBJECT
      public:
        void Initialize();
        void Draw();

        bool KeyPressed(QKeyEvent *);
        bool KeyReleased(QKeyEvent *);
        bool MousePressed(QMouseEvent *);
        bool MouseReleased(QMouseEvent *);
        bool MouseMoved(QMouseEvent *);

      signals:
        void GoToObject(Engine::ObjectPtr pObject);

      private:
        void DrawSun();
        void DrawSky();
        void DrawTerrain();
        void DrawHaze();
        void DrawRenderSettings();
        void DrawNodeInfo();
        void DrawStats();

        void DrawWorldPositionsWindow();

        void DrawObjectsWindow();
        void DrawObject(Engine::ObjectPtr pObject);
        void DrawObjectsTreeView();

        Engine::ObjectPtr mSelectedObject{ nullptr };

        Engine::DirectionalLightPtr mSun;
        Engine::SkyPtr mSky;
        Engine::TerrainPtr mTerrain;
        Engine::HazePtr mHaze;

        QVector3D mFragmentLocalPosition;
        QVector3D mFragmentWorldPosition;
        Engine::NodeInfo mNodeInfo;

        QVector<QVector3D> mSavedWorldPositions;
        int mSelectedWorldPositionIndex{ -1 };

        DEFINE_MEMBER_PTR(Engine::NodeManager, NodeManager);
        DEFINE_MEMBER_PTR(Engine::RenderingManager, RenderingManager);
    };

}