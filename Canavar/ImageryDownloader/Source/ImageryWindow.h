#pragma once

#include "ImageryDownloadManager.h"

#include <QOpenGLWidget>
#include <QtImGui.h>
#include <imgui.h>

#include <array>

class QTimer;

// Preconfigured tile servers.
struct TileServerPreset
{
    const char* name;
    const char* urlTemplate; // uses {z}, {x}, {y} placeholders
    const char* ext;         // file extension for saved tiles
    const char* description;
};

inline constexpr std::array<TileServerPreset, 4> kTileServerPresets = { {
    {
        "ESRI World Imagery (satellite)",
        "https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}",
        "jpg",
        "Global satellite/aerial imagery composite. No auth required.",
    },
    {
        "OpenStreetMap Standard",
        "https://tile.openstreetmap.org/{z}/{x}/{y}.png",
        "png",
        "OSM cartographic tiles. Bulk download discouraged per OSM tile policy.",
    },
    {
        "OpenTopoMap",
        "https://a.tile.opentopomap.org/{z}/{x}/{y}.png",
        "png",
        "Topographic rendering from SRTM + OSM data.",
    },
    {
        "Custom URL",
        "",
        "png",
        "Enter your own XYZ tile URL with {z}, {x}, {y} placeholders.",
    },
} };

class ImageryWindow : public QOpenGLWidget
{
    Q_OBJECT

  public:
    explicit ImageryWindow(QWidget* parent = nullptr);

  protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

  private:
    void buildImGui();
    void buildSettingsPanel();
    void buildQueuePanel();
    void buildZoomTable();
    void rebuildJobs();
    void startDownload();
    void openOutputDirDialog();

    QtImGui::RenderRef mImGuiRef{ nullptr };
    QTimer* mUpdateTimer{ nullptr };
    ImageryDownloadManager* mManager{ nullptr };

    // Settings
    int mPresetIndex{ 0 };
    char mCustomUrl[512]{};
    char mCustomExt[8]{ "png" };
    double mMinLat{ 36.0 };
    double mMaxLat{ 42.0 };
    double mMinLon{ 26.0 };
    double mMaxLon{ 30.0 };
    int mMinZoom{ 5 };
    int mMaxZoom{ 10 };
    char mOutputDir[1024]{};
    bool mSkipExisting{ true };
    int mConcurrency{ 8 };

    // Cached stats
    int mCachedDone{ 0 };
    int mCachedTotal{ 0 };

    bool mNeedsRebuild{ true };
    bool mRequestDirDialog{ false };
};
