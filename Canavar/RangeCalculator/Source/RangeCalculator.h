#pragma once

#include "CameraCalibration.h"

#include <Canavar/Engine/Core/Controller.h>
#include <Canavar/Engine/Core/EventReceiver.h>
#include <Canavar/Engine/Core/Window.h>
#include <Canavar/Engine/Manager/CameraManager.h>
#include <Canavar/Engine/Manager/NodeManager.h>
#include <Canavar/Engine/Manager/RenderingManager/RenderingManager.h>
#include <Canavar/Engine/Util/Logger.h>
#include <Canavar/Engine/Util/Macros.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <vector>

#include <QObject>
#include <QPointF>
#include <QThread>
#include <QTimer>

namespace RangeCalculator
{
    enum CameraType
    {
        Camera_0,
        Camera_1,
    };

    class RangeCalculator : public QObject, public Canavar::Engine::EventReceiver
    {
      public:
        explicit RangeCalculator(QObject *parent = nullptr);
        ~RangeCalculator();

        void Run();

        void Initialize() override;
        void Resize(int w, int h) override;
        void PostRender(float ifps) override;

        std::pair<QImage, QImage> RenderScene();
        void ExtractFeatures(const QImage &image0, const QImage &image1);
        void CalculateAveragePixelDelta();
        void CalculateRangeAndErrorRate();
        void DrawMatches();

        void DrawRangeLabel();

      private:
        cv::Mat Undistort(cv::Mat image);

        // Rendering related stuff
        Canavar::Engine::Controller *mController;
        Canavar::Engine::NodeManager *mNodeManager;
        Canavar::Engine::CameraManager *mCameraManager;
        Canavar::Engine::RenderingManager *mRenderingManager;
        Canavar::Engine::Window *mWindow;
        Canavar::Engine::PerspectiveCameraPtr mCamera;
        Canavar::Engine::PerspectiveCameraPtr mActiveCamera;
        QOpenGLFramebufferObjectFormat mFramebufferFormat;
        QOpenGLFramebufferObject *mFramebuffer{ nullptr };

        // Feature extraction stuff
        cv::Ptr<cv::ORB> mExtractor;
        cv::Ptr<cv::BFMatcher> mMatcher;

        std::vector<cv::KeyPoint> mKeyPoints[2];
        std::vector<cv::Point2d> mGoodKeyPoints[2];
        std::vector<cv::DMatch> mAllMatches;
        std::vector<cv::DMatch> mGoodMatches;

        float mGoodMatchRatio{ 0.1f };
        bool mCrossCheck{ true };

        float mDummyCameraHeadingAngle{ 0.0f };
        float mDistanceBetweenCameras{ 1.0f };
        bool mExtractFeatures{ false };
        bool mDrawMatches{ true };
        bool mDrawRangeLabel{ false };

        float mAveragePixelDelta{ 0 };
        float mDisparity{ 0 };

        float mCalculatedRange{ 0 };
        float mActualRange{ 0 };
        float mErrorRate{ 0 };
        float mDeltaAngle{ 0 };
        float mFocalLength{ 1.0f };
        float mSensorSize{ 900.0f };

        int mSavedFrameCount{ 0 };
        bool mSaveFrame{ false };

        bool mUndistortFrames{false};

        CameraCalibration mCameraCalibration;
    };
}
