#include "ImageryWindow.h"
#include "TileUtil.h"

#include <QDir>
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

ImageryWindow::ImageryWindow(QWidget* parent)
    : QOpenGLWidget(parent)
    , mManager(new ImageryDownloadManager(this))
{
    setMouseTracking(true);
    resize(1400, 900);

    mUpdateTimer = new QTimer(this);
    connect(mUpdateTimer, &QTimer::timeout, this, [this]() { update(); });
    mUpdateTimer->start(16);

    connect(mManager, &ImageryDownloadManager::progressChanged,
            this, [this](int done, int total) {
                mCachedDone = done;
                mCachedTotal = total;
                update();
            });

    connect(mManager, &ImageryDownloadManager::allFinished, this, [this]() { update(); });
}

// ---------------------------------------------------------------------------
// OpenGL lifecycle
// ---------------------------------------------------------------------------

void ImageryWindow::initializeGL()
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

void ImageryWindow::resizeGL(int /*w*/, int /*h*/) {}

void ImageryWindow::paintGL()
{
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

void ImageryWindow::buildImGui()
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

    ImGui::TextColored({ 0.40f, 1.0f, 0.65f, 1.0f }, "Satellite Imagery Tile Downloader");
    ImGui::TextDisabled("Downloads XYZ/TMS web map tiles for offline terrain texturing");
    ImGui::Separator();

    ImGui::BeginChild("##settings", { 370, 0 },
                      ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
    buildSettingsPanel();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("##queue", { 0, 0 }, ImGuiChildFlags_Borders);
    buildQueuePanel();
    ImGui::EndChild();

    ImGui::End();
}

// ---------------------------------------------------------------------------

void ImageryWindow::buildSettingsPanel()
{
    bool regionChanged = false;

    // ------------------------------------------------------------------
    ImGui::SeparatorText("Tile Server");

    const char* presetNames[kTileServerPresets.size() + 1];
    for (int i = 0; i < (int)kTileServerPresets.size(); ++i)
        presetNames[i] = kTileServerPresets[i].name;

    ImGui::SetNextItemWidth(-1);
    if (ImGui::Combo("##preset", &mPresetIndex, presetNames,
                     (int)kTileServerPresets.size()))
        regionChanged = true;

    ImGui::TextDisabled("%s", kTileServerPresets[mPresetIndex].description);

    // Custom URL input only for the last preset.
    const bool isCustom = (mPresetIndex == (int)kTileServerPresets.size() - 1);
    if (isCustom)
    {
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("URL template", mCustomUrl, sizeof(mCustomUrl)))
            regionChanged = true;
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputText("Ext", mCustomExt, sizeof(mCustomExt)))
            regionChanged = true;
        ImGui::SameLine();
        ImGui::TextDisabled("(jpg / png / webp)");
    }

    // Warning for OSM bulk-download policy.
    if (mPresetIndex == 1)
    {
        ImGui::Spacing();
        ImGui::TextColored({ 1.0f, 0.8f, 0.2f, 1.0f },
                           "Warning: OSM tile policy discourages\nbulk downloads. Use sparingly.");
    }

    // ------------------------------------------------------------------
    ImGui::SeparatorText("Geographic Region");

    static constexpr double kMinLat = -85.05, kMaxLat = 85.05;
    static constexpr double kMinLon = -180.0, kMaxLon = 180.0;

    ImGui::PushItemWidth(-1);
    regionChanged |= ImGui::SliderScalar("##minlat", ImGuiDataType_Double,
                                         &mMinLat, &kMinLat, &kMaxLat, "Min Lat  %.4f deg");
    regionChanged |= ImGui::SliderScalar("##maxlat", ImGuiDataType_Double,
                                         &mMaxLat, &kMinLat, &kMaxLat, "Max Lat  %.4f deg");
    regionChanged |= ImGui::SliderScalar("##minlon", ImGuiDataType_Double,
                                         &mMinLon, &kMinLon, &kMaxLon, "Min Lon  %.4f deg");
    regionChanged |= ImGui::SliderScalar("##maxlon", ImGuiDataType_Double,
                                         &mMaxLon, &kMinLon, &kMaxLon, "Max Lon  %.4f deg");
    ImGui::PopItemWidth();

    if (mMinLat > mMaxLat) std::swap(mMinLat, mMaxLat);
    if (mMinLon > mMaxLon) std::swap(mMinLon, mMaxLon);

    // ------------------------------------------------------------------
    ImGui::SeparatorText("Zoom Levels");

    ImGui::PushItemWidth(-1);
    regionChanged |= ImGui::SliderInt("##minz", &mMinZoom, 0, 18, "Min Zoom  %d");
    regionChanged |= ImGui::SliderInt("##maxz", &mMaxZoom, 0, 18, "Max Zoom  %d");
    ImGui::PopItemWidth();

    if (mMinZoom > mMaxZoom) std::swap(mMinZoom, mMaxZoom);

    buildZoomTable();

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

    ImGui::TextDisabled("Tiles saved as:  <dir>/{z}/{x}/{y}.%s",
                        isCustom ? mCustomExt : kTileServerPresets[mPresetIndex].ext);

    ImGui::Checkbox("Skip already-downloaded files", &mSkipExisting);
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##conc", &mConcurrency, 1, 16, "Connections: %d");

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
    if (mCachedTotal > 0)
    {
        ImGui::SeparatorText("Progress");
        const float pct = (float)mCachedDone / (float)mCachedTotal;
        char overlay[64];
        std::snprintf(overlay, sizeof(overlay), "%d / %d tiles", mCachedDone, mCachedTotal);
        ImGui::ProgressBar(pct, { -1, 0 }, overlay);

        // Per-status counts
        int done = 0, active = 0, pending = 0, failed = 0, skipped = 0;
        for (const auto& j : mManager->jobs())
        {
            switch (j.status)
            {
                case TileStatus::Done:    ++done;    break;
                case TileStatus::Active:  ++active;  break;
                case TileStatus::Pending: ++pending; break;
                case TileStatus::Failed:  ++failed;  break;
                case TileStatus::Skipped: ++skipped; break;
            }
        }
        ImGui::Text("Done: %d   Active: %d   Pending: %d", done, active, pending);
        if (failed > 0)
            ImGui::TextColored({ 1.0f, 0.4f, 0.4f, 1.0f }, "Failed: %d", failed);
        if (skipped > 0)
            ImGui::TextColored({ 1.0f, 0.8f, 0.2f, 1.0f }, "Skipped: %d", skipped);
    }
}

// ---------------------------------------------------------------------------

void ImageryWindow::buildZoomTable()
{
    constexpr ImGuiTableFlags kTableFlags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingFixedFit;

    if (!ImGui::BeginTable("##ztab", 3, kTableFlags))
        return;

    ImGui::TableSetupColumn("Zoom");
    ImGui::TableSetupColumn("Tiles");
    ImGui::TableSetupColumn("Est. Size");
    ImGui::TableHeadersRow();

    long long grandTotal = 0;
    for (int z = mMinZoom; z <= mMaxZoom; ++z)
    {
        const auto range = TileUtil::computeRange(mMinLat, mMaxLat, mMinLon, mMaxLon, z);
        const long long count = range.tileCount();
        grandTotal += count;
        const double estMB = count * 0.025; // ~25 KB average per tile

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Z%d", z);
        ImGui::TableNextColumn();

        // Colour-code high tile counts.
        if (count > 50'000)
            ImGui::TextColored({ 1.0f, 0.3f, 0.3f, 1.0f }, "%lld", count);
        else if (count > 10'000)
            ImGui::TextColored({ 1.0f, 0.8f, 0.2f, 1.0f }, "%lld", count);
        else
            ImGui::Text("%lld", count);

        ImGui::TableNextColumn();
        if (estMB >= 1000.0)
            ImGui::Text("%.2f GB", estMB / 1000.0);
        else
            ImGui::Text("%.1f MB", estMB);
    }

    // Grand total row
    ImGui::TableNextRow();
    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(60, 60, 80, 255));
    ImGui::TableNextColumn();
    ImGui::TextUnformatted("Total");
    ImGui::TableNextColumn();
    ImGui::Text("%lld", grandTotal);
    ImGui::TableNextColumn();
    const double totalMB = grandTotal * 0.025;
    if (totalMB >= 1000.0)
        ImGui::Text("%.2f GB", totalMB / 1000.0);
    else
        ImGui::Text("%.1f MB", totalMB);

    ImGui::EndTable();
}

// ---------------------------------------------------------------------------

void ImageryWindow::buildQueuePanel()
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
        ImGui::TextDisabled("Configure the region and zoom levels, then click 'Start Download'.");
        return;
    }

    constexpr int kMaxDisplay = 200'000;
    const int displayCount = std::min((int)jobs.size(), kMaxDisplay);

    ImGui::Text("%d tiles queued", (int)jobs.size());
    if ((int)jobs.size() > kMaxDisplay)
        ImGui::TextColored({ 1.0f, 0.8f, 0.2f, 1.0f },
                           "Showing first %d entries only.", kMaxDisplay);
    ImGui::Separator();

    ImGuiListClipper clipper;
    clipper.Begin(displayCount);
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            const auto& job = jobs[i];
            ImGui::PushID(i);

            ImVec4 badgeColor;
            const char* badge;
            switch (job.status)
            {
                case TileStatus::Pending: badgeColor = { 0.55f, 0.55f, 0.55f, 1.0f }; badge = "   "; break;
                case TileStatus::Active:  badgeColor = { 0.25f, 0.75f, 1.00f, 1.0f }; badge = " » "; break;
                case TileStatus::Done:    badgeColor = { 0.20f, 0.90f, 0.35f, 1.0f }; badge = " ✓ "; break;
                case TileStatus::Failed:  badgeColor = { 1.00f, 0.25f, 0.25f, 1.0f }; badge = " ✗ "; break;
                case TileStatus::Skipped: badgeColor = { 1.00f, 0.75f, 0.10f, 1.0f }; badge = " ~ "; break;
            }

            ImGui::TextColored(badgeColor, "%s", badge);
            ImGui::SameLine();
            ImGui::Text("Z%d / %d / %d", job.z, job.x, job.y);

            if (job.status == TileStatus::Active)
            {
                ImGui::SameLine();
                ImGui::TextDisabled("(downloading)");
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

void ImageryWindow::rebuildJobs()
{
    const bool isCustom = (mPresetIndex == (int)kTileServerPresets.size() - 1);
    const QString urlTemplate = isCustom
        ? QString(mCustomUrl)
        : QString(kTileServerPresets[mPresetIndex].urlTemplate);
    const QString ext = isCustom
        ? QString(mCustomExt)
        : QString(kTileServerPresets[mPresetIndex].ext);

    QVector<ImgTileJob> jobs;

    for (int z = mMinZoom; z <= mMaxZoom; ++z)
    {
        const auto range = TileUtil::computeRange(mMinLat, mMaxLat, mMinLon, mMaxLon, z);

        for (int x = range.xMin; x <= range.xMax; ++x)
        {
            for (int y = range.yMin; y <= range.yMax; ++y)
            {
                ImgTileJob job;
                job.z = z;
                job.x = x;
                job.y = y;
                job.url = TileUtil::buildUrl(urlTemplate, z, x, y);
                job.filePath = QString("%1/%2/%3/%4.%5")
                    .arg(mOutputDir)
                    .arg(z)
                    .arg(x)
                    .arg(y)
                    .arg(ext);

                if (mSkipExisting && QFile::exists(job.filePath))
                {
                    job.status = TileStatus::Skipped;
                    job.errorMsg = "Already exists";
                }

                jobs.append(job);
            }
        }
    }

    mCachedTotal = (int)jobs.size();
    mCachedDone = 0;
    mManager->setJobs(std::move(jobs));
}

void ImageryWindow::startDownload()
{
    if (mNeedsRebuild)
    {
        rebuildJobs();
        mNeedsRebuild = false;
    }
    mManager->start(mConcurrency);
}

void ImageryWindow::openOutputDirDialog()
{
    const QString current(mOutputDir);
    const QString dir = QFileDialog::getExistingDirectory(
        this, "Select Output Directory",
        current.isEmpty() ? QDir::homePath() : current);

    if (!dir.isEmpty())
    {
        const QByteArray bytes = dir.toLocal8Bit();
        std::snprintf(mOutputDir, sizeof(mOutputDir), "%s", bytes.constData());
        mNeedsRebuild = true;
        update();
    }
}
