#include "BuilderWindow.h"

#include <QFileDialog>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QTimer>

#include <imgui.h>
#include <QtImGui.h>

#include <cstdio>

namespace Canavar::Globe
{
    BuilderWindow::BuilderWindow(QWidget* Parent)
        : QOpenGLWidget(Parent)
    {
        setMouseTracking(true);
        resize(1400, 900);

        mUpdateTimer = new QTimer(this);
        connect(mUpdateTimer, &QTimer::timeout, this, [this]() { update(); });
        mUpdateTimer->start(16);
    }

    BuilderWindow::~BuilderWindow()
    {
        if (mBuildThread && mBuildThread->isRunning())
        {
            if (mBuilder) mBuilder->RequestAbort();
            mBuildThread->wait(5000);
        }
    }

    void BuilderWindow::initializeGL()
    {
        mImGuiRef = QtImGui::initialize(this);
        ImGui::StyleColorsDark();
        ImGuiStyle& Style = ImGui::GetStyle();
        Style.WindowRounding = 0.0f;
        Style.ChildRounding  = 4.0f;
        Style.FrameRounding  = 4.0f;
        Style.Colors[ImGuiCol_WindowBg] = { 0.10f, 0.10f, 0.12f, 1.0f };
        ImGui::GetIO().IniFilename = nullptr;
    }

    void BuilderWindow::resizeGL(int /*W*/, int /*H*/) {}

    void BuilderWindow::paintGL()
    {
        if (mRequestDirDialog)
        {
            mRequestDirDialog = false;
            QTimer::singleShot(0, this, [this]() {
                OpenFolderDialog(mDirDialogTarget, 1024, mDirDialogTitle);
            });
        }

        auto* F = context()->functions();
        F->glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
        F->glClear(GL_COLOR_BUFFER_BIT);

        QtImGui::newFrame(mImGuiRef);
        BuildImGui();
        ImGui::Render();
        QtImGui::render(mImGuiRef);
    }

    void BuilderWindow::BuildImGui()
    {
        const ImGuiIO& IO = ImGui::GetIO();
        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize(IO.DisplaySize);

        constexpr ImGuiWindowFlags kFlags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("##root", nullptr, kFlags);

        ImGui::TextColored({ 1.0f, 0.8f, 0.3f, 1.0f }, "Terrain Builder  –  WGS-84 Quadtree LOD Pipeline");
        ImGui::TextDisabled("Generates .tbin terrain tiles from GLO-30 DTED + XYZ satellite imagery");
        ImGui::Separator();

        ImGui::BeginChild("##left", { 380, 0 }, ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
        BuildSettingsPanel();
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##right", { 0, 0 }, ImGuiChildFlags_Borders);
        BuildProgressPanel();
        ImGui::EndChild();

        ImGui::End();
    }

    void BuilderWindow::BuildSettingsPanel()
    {
        ImGui::SeparatorText("Input / Output Folders");

        auto FolderRow = [&](const char* Label, char* Buf, const char* Title)
        {
            ImGui::TextUnformatted(Label);
            const std::string InputId = std::string("##folder_") + Label;
            const std::string BtnId   = std::string("Browse##") + Label;
            ImGui::SetNextItemWidth(-84);
            ImGui::InputText(InputId.c_str(), Buf, 1024, ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            if (ImGui::Button(BtnId.c_str(), { 76, 0 }))
            {
                mDirDialogTarget = Buf;
                mDirDialogTitle  = Title;
                mRequestDirDialog = true;
            }
        };

        FolderRow("DTED (GLO-30)",    mDtedFolder,   "Select DTED folder (GLO-30 .tif files)");
        FolderRow("Albedo (imagery)", mAlbedoFolder, "Select Albedo folder ({z}/{x}/{y}.jpg)");
        FolderRow("Output",           mOutputFolder, "Select output folder for terrain tiles");

        ImGui::SeparatorText("Geographic Region");

        ImGui::PushItemWidth(-1);
        ImGui::SliderFloat("##minlat", &mMinLat, -90.0f,  90.0f, "Min Lat  %.2f deg");
        ImGui::SliderFloat("##maxlat", &mMaxLat, -90.0f,  90.0f, "Max Lat  %.2f deg");
        ImGui::SliderFloat("##minlon", &mMinLon, -180.0f, 180.0f,"Min Lon  %.2f deg");
        ImGui::SliderFloat("##maxlon", &mMaxLon, -180.0f, 180.0f,"Max Lon  %.2f deg");
        ImGui::PopItemWidth();

        if (mMinLat > mMaxLat) std::swap(mMinLat, mMaxLat);
        if (mMinLon > mMaxLon) std::swap(mMinLon, mMaxLon);

        ImGui::SeparatorText("LOD Levels");

        ImGui::PushItemWidth(-1);
        ImGui::SliderInt("##minlevel", &mMinLevel, 0, 12, "Min Level  %d");
        ImGui::SliderInt("##maxlevel", &mMaxLevel, 0, 12, "Max Level  %d");
        ImGui::PopItemWidth();
        if (mMinLevel > mMaxLevel) std::swap(mMinLevel, mMaxLevel);

        // Tile count estimate
        long long TileCount = 0;
        for (int L = mMinLevel; L <= mMaxLevel; ++L)
        {
            const double W = TileWidthDeg(L), H = TileHeightDeg(L);
            const long long NX = static_cast<long long>((mMaxLon - mMinLon) / W) + 1;
            const long long NY = static_cast<long long>((mMaxLat - mMinLat) / H) + 1;
            TileCount += NX * NY;
        }
        ImGui::Text("Estimated tiles: %lld", TileCount);

        ImGui::SeparatorText("Quality");

        ImGui::PushItemWidth(-1);
        ImGui::SliderInt("##imgzoom",   &mImageryZoom,  0, 18, "Imagery Zoom    %d");
        ImGui::SliderInt("##texsize",   &mTextureSize, 64, 2048,"Texture Size  %d px");
        ImGui::SliderInt("##jpegq",     &mJpegQuality,  1, 100, "JPEG Quality    %d");
        ImGui::SliderInt("##meshres",   &mMeshGridRes, 17, 129,  "Mesh Grid Res   %d");
        ImGui::PopItemWidth();
        ImGui::Checkbox("Skip already-built tiles", &mSkipExisting);

        ImGui::SeparatorText("Control");

        const bool CanStart = !mBuilding
            && mDtedFolder[0] != '\0'
            && mAlbedoFolder[0] != '\0'
            && mOutputFolder[0] != '\0';

        ImGui::BeginDisabled(!CanStart);
        if (ImGui::Button("Start Build", { -1, 36 }))
            StartBuild();
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!mBuilding);
        if (ImGui::Button("Abort", { -1, 0 }) && mBuilder)
            mBuilder->RequestAbort();
        ImGui::EndDisabled();

        if (mBuilding && mTotalTiles > 0)
        {
            ImGui::SeparatorText("Progress");
            char Overlay[64];
            std::snprintf(Overlay, sizeof(Overlay), "%d / %d", mDoneTiles, mTotalTiles);
            ImGui::ProgressBar(static_cast<float>(mDoneTiles) / mTotalTiles, { -1, 0 }, Overlay);
        }
    }

    void BuilderWindow::BuildProgressPanel()
    {
        ImGui::SeparatorText("Build Log");

        ImGui::BeginChild("##log", { 0, 0 }, ImGuiChildFlags_None,
                          ImGuiWindowFlags_HorizontalScrollbar);

        ImGuiListClipper Clipper;
        Clipper.Begin(static_cast<int>(mLog.size()));
        while (Clipper.Step())
            for (int I = Clipper.DisplayStart; I < Clipper.DisplayEnd; ++I)
                ImGui::TextUnformatted(mLog[I].toUtf8().constData());
        Clipper.End();

        // Auto-scroll to bottom
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
    }

    void BuilderWindow::StartBuild()
    {
        mLog.clear();
        mDoneTiles  = 0;
        mTotalTiles = 0;
        mBuilding   = true;

        BuildSettings Settings;
        Settings.DtedFolder   = mDtedFolder;
        Settings.AlbedoFolder = mAlbedoFolder;
        Settings.OutputFolder = mOutputFolder;
        Settings.Region       = { mMinLat, mMaxLat, mMinLon, mMaxLon };
        Settings.MinLevel     = mMinLevel;
        Settings.MaxLevel     = mMaxLevel;
        Settings.MeshGridRes  = mMeshGridRes;
        Settings.ImageryZoom  = mImageryZoom;
        Settings.TextureSize  = mTextureSize;
        Settings.JpegQuality  = mJpegQuality;
        Settings.SkipExisting = mSkipExisting;

        mBuildThread = new QThread(this);
        mBuilder     = new QuadtreeBuilder;

        mBuilder->moveToThread(mBuildThread);

        // Capture raw pointers by value so the lambdas are independent of member state.
        auto* Thread  = mBuildThread;
        auto* Builder = mBuilder;

        connect(Thread, &QThread::started, Builder,
                [Builder, Settings]() { Builder->Build(Settings); });

        connect(Builder, &QuadtreeBuilder::ProgressMessage, this,
                [this](const QString& Msg)
                {
                    mLog.append(Msg);
                    if (mLog.size() > 2000) mLog.removeFirst();
                    update();
                }, Qt::QueuedConnection);

        connect(Builder, &QuadtreeBuilder::TileCompleted, this,
                [this](int Done, int Total)
                {
                    mDoneTiles  = Done;
                    mTotalTiles = Total;
                    update();
                }, Qt::QueuedConnection);

        // Stop the thread via a direct signal→slot connection (no lambda touching members).
        connect(Builder, &QuadtreeBuilder::BuildFinished, Thread, &QThread::quit,
                Qt::QueuedConnection);

        connect(Builder, &QuadtreeBuilder::BuildFinished, this,
                [this](bool Success)
                {
                    mBuilding = false;
                    mLog.append(Success ? "=== Build finished successfully ==="
                                        : "=== Build failed or was aborted ===");
                    update();
                }, Qt::QueuedConnection);

        connect(Thread, &QThread::finished, Builder, &QObject::deleteLater);
        connect(Thread, &QThread::finished, Thread,  &QObject::deleteLater);
        // Clear member pointers once the objects are gone to prevent dangling access.
        connect(Thread, &QThread::finished, this,
                [this]() { mBuildThread = nullptr; mBuilder = nullptr; },
                Qt::QueuedConnection);

        Thread->start();
    }

    void BuilderWindow::OpenFolderDialog(char* Buffer, int BufferSize, const char* Title)
    {
        const QString Dir = QFileDialog::getExistingDirectory(
            this, Title, QString(Buffer).isEmpty() ? QDir::homePath() : QString(Buffer));
        if (!Dir.isEmpty())
        {
            const QByteArray Bytes = Dir.toLocal8Bit();
            std::snprintf(Buffer, BufferSize, "%s", Bytes.constData());
            update();
        }
    }

} // namespace Canavar::Globe
