#include "DtedWindow.h"
#include "Glo30Util.h"

#include <QFile>
#include <QFileDialog>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QTimer>

#include <imgui.h>
#include <QtImGui.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

DtedWindow::DtedWindow(QWidget* parent)
    : QOpenGLWidget(parent)
    , mManager(new DtedDownloadManager(this))
{
    setMouseTracking(true);
    resize(1400, 900);

    mUpdateTimer = new QTimer(this);
    connect(mUpdateTimer, &QTimer::timeout, this, [this]() { update(); });
    mUpdateTimer->start(16);

    connect(mManager, &DtedDownloadManager::progressChanged, this, [this]() { update(); });
    connect(mManager, &DtedDownloadManager::allFinished, this, [this]() { update(); });
}

// ---------------------------------------------------------------------------
// OpenGL lifecycle
// ---------------------------------------------------------------------------

void DtedWindow::initializeGL()
{
    mImGuiRef = QtImGui::initialize(this);

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.Colors[ImGuiCol_WindowBg] = { 0.10f, 0.10f, 0.12f, 1.0f };

    ImGui::GetIO().IniFilename = nullptr;
}

void DtedWindow::resizeGL(int /*w*/, int /*h*/) {}

void DtedWindow::paintGL()
{
    // Defer file dialog until after GL commands finish.
    if (mRequestDirDialog)
    {
        mRequestDirDialog = false;
        QTimer::singleShot(0, this, [this]() { openOutputDirDialog(); });
    }

    auto* f = context()->functions();
    f->glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
    f->glClear(GL_COLOR_BUFFER_BIT);

    QtImGui::newFrame(mImGuiRef);
    buildImGui();
    ImGui::Render();
    QtImGui::render(mImGuiRef);
}

// ---------------------------------------------------------------------------
// ImGui layout
// ---------------------------------------------------------------------------

void DtedWindow::buildImGui()
{
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize(io.DisplaySize);

    constexpr ImGuiWindowFlags kRootFlags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("##root", nullptr, kRootFlags);

    ImGui::TextColored({ 0.40f, 0.85f, 1.0f, 1.0f },
                       "Copernicus GLO-30 DEM Downloader");
    ImGui::TextDisabled("1-arc-second (30 m) global elevation tiles from AWS Open Data");
    ImGui::Separator();

    // Left panel – settings (resizable)
    ImGui::BeginChild("##settings", { 340, 0 },
                      ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
    buildSettingsPanel();
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel – download queue
    ImGui::BeginChild("##queue", { 0, 0 }, ImGuiChildFlags_Borders);
    buildQueuePanel();
    ImGui::EndChild();

    ImGui::End();
}

// ---------------------------------------------------------------------------

void DtedWindow::buildSettingsPanel()
{
    bool regionChanged = false;

    ImGui::SeparatorText("Geographic Region  (1 deg tiles)");

    ImGui::PushItemWidth(-1);
    regionChanged |= ImGui::SliderInt("##minlat", &mMinLat, -90, 89, "Min Lat  %d deg");
    regionChanged |= ImGui::SliderInt("##maxlat", &mMaxLat, -90, 89, "Max Lat  %d deg");
    regionChanged |= ImGui::SliderInt("##minlon", &mMinLon, -180, 179, "Min Lon  %d deg");
    regionChanged |= ImGui::SliderInt("##maxlon", &mMaxLon, -180, 179, "Max Lon  %d deg");
    ImGui::PopItemWidth();

    if (mMinLat > mMaxLat) std::swap(mMinLat, mMaxLat);
    if (mMinLon > mMaxLon) std::swap(mMinLon, mMaxLon);

    const int latCount = mMaxLat - mMinLat + 1;
    const int lonCount = mMaxLon - mMinLon + 1;
    const int tileCount = latCount * lonCount;
    const float estGB = tileCount * 0.040f; // ~40 MB average per GLO-30 tile

    ImGui::Text("Tiles: %d  |  Est. size: %.1f GB", tileCount, estGB);

    if (regionChanged)
        mNeedsRebuild = true;

    // ------------------------------------------------------------------
    ImGui::SeparatorText("Output");

    ImGui::SetNextItemWidth(-84);
    ImGui::InputText("##dir", mOutputDir, sizeof(mOutputDir),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    if (ImGui::Button("Browse", { 76, 0 }))
        mRequestDirDialog = true;

    ImGui::Checkbox("Skip already-downloaded files", &mSkipExisting);

    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##conc", &mConcurrency, 1, 8, "Connections: %d");

    // ------------------------------------------------------------------
    ImGui::SeparatorText("Control");

    const bool running = mManager->isRunning();
    const bool canStart = !running && mOutputDir[0] != '\0';

    ImGui::BeginDisabled(!canStart);
    if (ImGui::Button("Start Download", { -1, 36 }))
        startDownload();
    ImGui::EndDisabled();

    ImGui::BeginDisabled(!running);
    if (ImGui::Button("Abort", { -1, 0 }))
        mManager->abort();
    ImGui::EndDisabled();

    // ------------------------------------------------------------------
    const auto& jobs = mManager->jobs();
    if (!jobs.isEmpty())
    {
        int done = 0, active = 0, failed = 0, skipped = 0, pending = 0;
        for (const auto& j : jobs)
        {
            switch (j.status)
            {
                case JobStatus::Done:    ++done;    break;
                case JobStatus::Active:  ++active;  break;
                case JobStatus::Failed:  ++failed;  break;
                case JobStatus::Skipped: ++skipped; break;
                case JobStatus::Pending: ++pending; break;
            }
        }

        ImGui::SeparatorText("Progress");

        const int finished = done + skipped + failed;
        const float pct = (float)finished / (float)jobs.size();
        char overlay[64];
        std::snprintf(overlay, sizeof(overlay), "%d / %d", finished, (int)jobs.size());
        ImGui::ProgressBar(pct, { -1, 0 }, overlay);

        ImGui::Text("Done: %d   Active: %d   Pending: %d", done, active, pending);
        if (failed > 0)
            ImGui::TextColored({ 1.0f, 0.4f, 0.4f, 1.0f }, "Failed: %d", failed);
        if (skipped > 0)
            ImGui::TextColored({ 1.0f, 0.8f, 0.2f, 1.0f }, "Skipped (no tile): %d", skipped);
    }
}

// ---------------------------------------------------------------------------

void DtedWindow::buildQueuePanel()
{
    ImGui::SeparatorText("Download Queue");

    if (mNeedsRebuild && !mManager->isRunning())
    {
        rebuildJobs();
        mNeedsRebuild = false;
    }

    const auto& jobs = mManager->jobs();

    if (jobs.isEmpty())
    {
        ImGui::TextDisabled("Adjust the region on the left then click 'Start Download'.");
        return;
    }

    ImGui::Text("%d tiles queued", (int)jobs.size());
    ImGui::Separator();

    ImGuiListClipper clipper;
    clipper.Begin((int)jobs.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            const auto& job = jobs[i];
            ImGui::PushID(i);

            // Status badge
            ImVec4 badgeColor;
            const char* badge;
            switch (job.status)
            {
                case JobStatus::Pending: badgeColor = { 0.55f, 0.55f, 0.55f, 1.0f }; badge = "   "; break;
                case JobStatus::Active:  badgeColor = { 0.25f, 0.75f, 1.00f, 1.0f }; badge = " » "; break;
                case JobStatus::Done:    badgeColor = { 0.20f, 0.90f, 0.35f, 1.0f }; badge = " ✓ "; break;
                case JobStatus::Failed:  badgeColor = { 1.00f, 0.25f, 0.25f, 1.0f }; badge = " ✗ "; break;
                case JobStatus::Skipped: badgeColor = { 1.00f, 0.75f, 0.10f, 1.0f }; badge = " ~ "; break;
            }

            ImGui::TextColored(badgeColor, "%s", badge);
            ImGui::SameLine();

            // Tile coordinate label
            char ns = job.lat >= 0 ? 'N' : 'S';
            char ew = job.lon >= 0 ? 'E' : 'W';
            ImGui::Text("%c%02d %c%03d", ns, std::abs(job.lat), ew, std::abs(job.lon));

            // Inline progress / info
            if (job.status == JobStatus::Active && job.bytesTotal > 0)
            {
                ImGui::SameLine();
                const float p = (float)job.bytesReceived / (float)job.bytesTotal;
                ImGui::ProgressBar(p, { 120, 0 });
                ImGui::SameLine();
                ImGui::TextDisabled("%.1f / %.1f MB",
                                    job.bytesReceived / 1e6,
                                    job.bytesTotal / 1e6);
            }
            else if (job.status == JobStatus::Done && job.bytesTotal > 0)
            {
                ImGui::SameLine();
                ImGui::TextDisabled("%.1f MB", job.bytesTotal / 1e6);
            }
            else if (!job.errorMsg.isEmpty())
            {
                ImGui::SameLine();
                ImGui::TextColored({ 0.9f, 0.5f, 0.5f, 1.0f },
                                   "(%s)", job.errorMsg.toUtf8().constData());
            }

            ImGui::PopID();
        }
    }
    clipper.End();
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void DtedWindow::rebuildJobs()
{
    QVector<DtedTileJob> jobs;

    for (int lat = mMinLat; lat <= mMaxLat; ++lat)
    {
        for (int lon = mMinLon; lon <= mMaxLon; ++lon)
        {
            DtedTileJob job;
            job.lat = lat;
            job.lon = lon;
            job.url = Glo30Util::buildUrl(lat, lon);
            job.filePath = QString("%1/%2").arg(mOutputDir, Glo30Util::buildFilename(lat, lon));

            if (mSkipExisting && QFile::exists(job.filePath))
            {
                job.status = JobStatus::Skipped;
                job.errorMsg = "Already exists";
            }

            jobs.append(job);
        }
    }

    mManager->setJobs(std::move(jobs));
}

void DtedWindow::startDownload()
{
    if (mNeedsRebuild)
    {
        rebuildJobs();
        mNeedsRebuild = false;
    }
    mManager->start(mConcurrency);
}

void DtedWindow::openOutputDirDialog()
{
    const QString current(mOutputDir);
    const QString dir = QFileDialog::getExistingDirectory(
        this, "Select Output Directory", current.isEmpty() ? QDir::homePath() : current);

    if (!dir.isEmpty())
    {
        const QByteArray bytes = dir.toLocal8Bit();
        std::snprintf(mOutputDir, sizeof(mOutputDir), "%s", bytes.constData());
        mNeedsRebuild = true;
        update();
    }
}
