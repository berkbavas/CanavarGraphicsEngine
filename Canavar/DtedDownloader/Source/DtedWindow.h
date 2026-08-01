#pragma once

#include "DtedDownloadManager.h"

#include <QOpenGLWidget>
#include <QtImGui.h>
#include <imgui.h>

class QTimer;

class DtedWindow : public QOpenGLWidget
{
    Q_OBJECT

  public:
    explicit DtedWindow(QWidget* parent = nullptr);

  protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

  private:
    void buildImGui();
    void buildSettingsPanel();
    void buildQueuePanel();
    void rebuildJobs();
    void startDownload();
    void openOutputDirDialog();

    QtImGui::RenderRef mImGuiRef{ nullptr };
    QTimer* mUpdateTimer{ nullptr };
    DtedDownloadManager* mManager{ nullptr };

    // Settings state
    int mMinLat{ 36 };
    int mMaxLat{ 42 };
    int mMinLon{ 26 };
    int mMaxLon{ 30 };
    char mOutputDir[1024]{};
    bool mSkipExisting{ true };
    int mConcurrency{ 4 };

    // Internal flags
    bool mNeedsRebuild{ true };
    bool mRequestDirDialog{ false };
};
