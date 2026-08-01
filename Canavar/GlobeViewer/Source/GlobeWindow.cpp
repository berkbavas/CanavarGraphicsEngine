#include "GlobeWindow.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QWheelEvent>

#include <imgui.h>
#include <QtImGui.h>

#include <cstdio>

namespace Canavar::Globe
{
    GlobeWindow::GlobeWindow(const QString& TerrainRoot, QWidget* Parent)
        : QOpenGLWidget(Parent)
        , mTerrainRoot(TerrainRoot)
    {
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);
        resize(1600, 900);

        mUpdateTimer = new QTimer(this);
        connect(mUpdateTimer, &QTimer::timeout, this, [this]() { update(); });
        mUpdateTimer->start(16);

        mFrameTimer.start();
    }

    void GlobeWindow::initializeGL()
    {
        mImGuiRef = QtImGui::initialize(this);

        ImGui::StyleColorsDark();
        ImGuiStyle& Style = ImGui::GetStyle();
        Style.WindowRounding = 4.0f;
        Style.ChildRounding  = 4.0f;
        Style.FrameRounding  = 4.0f;
        ImGui::GetIO().IniFilename = nullptr;

        mTerrain = std::make_unique<GlobeTerrainManager>(mTerrainRoot);
        mTerrainReady = mTerrain->Initialize();

        auto* F = context()->functions();
        F->glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
        F->glEnable(GL_DEPTH_TEST);
    }

    void GlobeWindow::resizeGL(int W, int H)
    {
        if (H > 0) mCamera.SetAspectRatio(static_cast<float>(W) / static_cast<float>(H));
    }

    void GlobeWindow::paintGL()
    {
        const double DtSec = static_cast<double>(mFrameTimer.elapsed()) / 1000.0;
        mFrameTimer.restart();

        HandleCameraInput(DtSec);

        auto* F = context()->functions();
        F->glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
        F->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (mTerrainReady)
            mTerrain->Render(mCamera, width(), height());

        QtImGui::newFrame(mImGuiRef);
        BuildImGui();
        ImGui::Render();
        QtImGui::render(mImGuiRef);
    }

    void GlobeWindow::HandleCameraInput(double DtSec)
    {
        if (ImGui::GetIO().WantCaptureKeyboard) return;

        const double MoveSpeed = mCamera.Altitude() * 0.5 * DtSec; // proportional to altitude

        // Real-time key state via ImGui IO (works without keyPressEvent repeat)
        if (ImGui::IsKeyDown(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_UpArrow))
            mCamera.MoveForward(MoveSpeed);
        if (ImGui::IsKeyDown(ImGuiKey_S) || ImGui::IsKeyDown(ImGuiKey_DownArrow))
            mCamera.MoveForward(-MoveSpeed);
        if (ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_LeftArrow))
            mCamera.MoveRight(-MoveSpeed);
        if (ImGui::IsKeyDown(ImGuiKey_D) || ImGui::IsKeyDown(ImGuiKey_RightArrow))
            mCamera.MoveRight(MoveSpeed);
        if (ImGui::IsKeyDown(ImGuiKey_Q))
            mCamera.AdjustAltitude(-MoveSpeed);
        if (ImGui::IsKeyDown(ImGuiKey_E))
            mCamera.AdjustAltitude(MoveSpeed);
    }

    void GlobeWindow::BuildImGui()
    {
        // ── Camera HUD (top-left, semi-transparent) ───────────────────────────
        ImGui::SetNextWindowPos({ 10, 10 });
        ImGui::SetNextWindowSize({ 320, 0 });
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin("Camera", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse);

        // Editable position
        mLatInput = static_cast<float>(mCamera.Latitude());
        mLonInput = static_cast<float>(mCamera.Longitude());
        mAltInput = static_cast<float>(mCamera.Altitude());
        mHeading  = static_cast<float>(mCamera.Heading());
        mPitch    = static_cast<float>(mCamera.Pitch());

        bool Changed = false;
        ImGui::PushItemWidth(-1);
        Changed |= ImGui::SliderFloat("##lat",     &mLatInput, -89.9f,  89.9f, "Lat  %.4f deg");
        Changed |= ImGui::SliderFloat("##lon",     &mLonInput, -180.0f, 180.0f,"Lon  %.4f deg");
        Changed |= ImGui::SliderFloat("##alt",     &mAltInput, 10.0f, 20000000.0f, "Alt  %.0f m", ImGuiSliderFlags_Logarithmic);
        Changed |= ImGui::SliderFloat("##heading", &mHeading,  0.0f,   360.0f,"Hdg  %.1f deg");
        Changed |= ImGui::SliderFloat("##pitch",   &mPitch,   -89.0f,   89.0f,"Pitch %.1f deg");
        ImGui::PopItemWidth();

        if (Changed)
        {
            mCamera.SetGeoPosition(mLatInput, mLonInput, mAltInput);
            mCamera.SetHeading(mHeading);
            mCamera.SetPitch(mPitch);
        }

        ImGui::Separator();
        ImGui::TextDisabled("WASD/arrows = move  |  Q/E = altitude");
        ImGui::TextDisabled("Right-drag = look   |  Scroll = altitude");

        ImGui::End();

        // ── Terrain info (top-right) ──────────────────────────────────────────
        ImGui::SetNextWindowPos({ static_cast<float>(width()) - 260.0f, 10.0f });
        ImGui::SetNextWindowSize({ 250, 0 });
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin("Terrain", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse);

        if (!mTerrainReady)
        {
            ImGui::TextColored({ 1, 0.3f, 0.3f, 1 }, "Index not loaded.");
            ImGui::TextDisabled("Set terrain root and restart.");
        }
        else
        {
            ImGui::Text("Loaded tiles: %d", mTerrain ? mTerrain->GetLoadedTileCount() : 0);

            float Thresh = mTerrain ? mTerrain->GetPixelErrorThreshold() : 4.0f;
            if (ImGui::SliderFloat("SSE px", &Thresh, 0.5f, 32.0f) && mTerrain)
                mTerrain->SetPixelErrorThreshold(Thresh);

            bool Wire = mTerrain && mTerrain->GetWireframe();
            if (ImGui::Checkbox("Wireframe", &Wire) && mTerrain)
                mTerrain->SetWireframe(Wire);
        }

        ImGui::End();
    }

    // ── Input events ─────────────────────────────────────────────────────────

    void GlobeWindow::keyPressEvent(QKeyEvent* Event)
    {
        QOpenGLWidget::keyPressEvent(Event);
    }

    void GlobeWindow::wheelEvent(QWheelEvent* Event)
    {
        if (!ImGui::GetIO().WantCaptureMouse)
        {
            const double Delta = Event->angleDelta().y();
            mCamera.AdjustAltitude(-mCamera.Altitude() * Delta / 1200.0);
        }
        QOpenGLWidget::wheelEvent(Event);
    }

    void GlobeWindow::mousePressEvent(QMouseEvent* Event)
    {
        if (Event->button() == Qt::RightButton && !ImGui::GetIO().WantCaptureMouse)
        {
            mDragging = true;
            mLastMousePos = Event->position();
        }
        QOpenGLWidget::mousePressEvent(Event);
    }

    void GlobeWindow::mouseMoveEvent(QMouseEvent* Event)
    {
        if (mDragging && !ImGui::GetIO().WantCaptureMouse)
        {
            const QPointF Delta = Event->position() - mLastMousePos;
            mCamera.RotateHeading(Delta.x() * 0.2);
            mCamera.RotatePitch(Delta.y() * 0.15);
            mLastMousePos = Event->position();
        }
        QOpenGLWidget::mouseMoveEvent(Event);
    }

    void GlobeWindow::mouseReleaseEvent(QMouseEvent* Event)
    {
        if (Event->button() == Qt::RightButton)
            mDragging = false;
        QOpenGLWidget::mouseReleaseEvent(Event);
    }

} // namespace Canavar::Globe
