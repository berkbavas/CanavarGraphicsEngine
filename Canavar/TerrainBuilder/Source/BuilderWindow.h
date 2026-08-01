#pragma once

#include "QuadtreeBuilder.h"

#include <QOpenGLWidget>
#include <QThread>
#include <QVector>
#include <QtImGui.h>
#include <imgui.h>

class QTimer;

namespace Canavar::Globe
{
    class BuilderWindow : public QOpenGLWidget
    {
        Q_OBJECT

      public:
        explicit BuilderWindow(QWidget* Parent = nullptr);
        ~BuilderWindow() override;

      protected:
        void initializeGL() override;
        void paintGL() override;
        void resizeGL(int W, int H) override;

      private:
        void BuildImGui();
        void BuildSettingsPanel();
        void BuildProgressPanel();
        void StartBuild();
        void OpenFolderDialog(char* Buffer, int BufferSize, const char* Title);

        QtImGui::RenderRef mImGuiRef{ nullptr };
        QTimer*            mUpdateTimer{ nullptr };

        // ── build settings ──────────────────────────────────────────────────
        char mDtedFolder[1024]{};
        char mAlbedoFolder[1024]{};
        char mOutputFolder[1024]{};
        float mMinLat{ 36.0f }, mMaxLat{ 42.0f };
        float mMinLon{ 26.0f }, mMaxLon{ 45.0f };
        int   mMinLevel{ 2 }, mMaxLevel{ 8 };
        int   mImageryZoom{ 12 };
        int   mTextureSize{ 512 };
        int   mJpegQuality{ 85 };
        int   mMeshGridRes{ 65 };
        bool  mSkipExisting{ true };
        bool  mRequestDirDialog{ false };
        char* mDirDialogTarget{ nullptr };
        const char* mDirDialogTitle{ "" };

        // ── runtime state ───────────────────────────────────────────────────
        QThread*         mBuildThread{ nullptr };
        QuadtreeBuilder* mBuilder{ nullptr };
        bool             mBuilding{ false };
        int              mDoneTiles{ 0 };
        int              mTotalTiles{ 0 };
        QVector<QString> mLog;
    };

} // namespace Canavar::Globe
