#pragma once

#include "GlobeCamera.h"
#include "GlobeTerrainManager.h"

#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QtImGui.h>
#include <imgui.h>

#include <memory>

class QTimer;
class QKeyEvent;
class QWheelEvent;

namespace Canavar::Globe
{
    class GlobeWindow : public QOpenGLWidget
    {
        Q_OBJECT

      public:
        explicit GlobeWindow(const QString& TerrainRoot, QWidget* Parent = nullptr);
        ~GlobeWindow() override = default;

      protected:
        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int W, int H) override;

        void keyPressEvent(QKeyEvent* Event) override;
        void wheelEvent(QWheelEvent* Event) override;
        void mousePressEvent(QMouseEvent* Event) override;
        void mouseMoveEvent(QMouseEvent* Event) override;
        void mouseReleaseEvent(QMouseEvent* Event) override;

      private:
        void BuildImGui();
        void HandleCameraInput(double DtSec);

        QtImGui::RenderRef mImGuiRef{ nullptr };
        QTimer*            mUpdateTimer{ nullptr };
        QElapsedTimer      mFrameTimer;

        GlobeCamera                       mCamera;
        std::unique_ptr<GlobeTerrainManager> mTerrain;
        QString mTerrainRoot;
        bool    mTerrainReady{ false };

        // ImGui controls
        float mLatInput{ 41.0f }, mLonInput{ 29.0f }, mAltInput{ 5000.0f };
        float mHeading{ 0.0f },   mPitch{ -45.0f };

        // Mouse drag state
        bool    mDragging{ false };
        QPointF mLastMousePos;
    };

} // namespace Canavar::Globe
