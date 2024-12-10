#include "RangeCalculator.h"

#include <Canavar/Engine/Node/Object/Camera/DummyCamera.h>
#include <imgui.h>

#include <QPainter>
#include <QtImGui.h>

RangeCalculator::RangeCalculator::RangeCalculator(QObject *parent)
    : QObject(parent)
{
    mController = new Canavar::Engine::Controller;
    mController->AddEventReceiver(this);

    mFramebufferFormat.setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    mFramebufferFormat.setSamples(0);
}

RangeCalculator::RangeCalculator::~RangeCalculator()
{
    mController->RemoveEventReceiver(this);
    mExtractFeatures = false;
    mDrawMatches = false;
    mDrawRangeLabel = false;
}

void RangeCalculator::RangeCalculator::Run()
{
    mController->Run();

    // mCameraCalibration.Calibrate();
}

void RangeCalculator::RangeCalculator::Initialize()
{
    mNodeManager = mController->GetNodeManager();
    mCameraManager = mController->GetCameraManager();
    mRenderingManager = mController->GetRenderingManager();
    mWindow = mController->GetWindow();

    mNodeManager->ImportNodes("Resources/Checkerboard.json");
    mNodeManager->GetTerrain()->SetEnabled(false);
    mNodeManager->GetSky()->SetEnabled(true);

    mActiveCamera = std::dynamic_pointer_cast<Canavar::Engine::PerspectiveCamera>(mCameraManager->GetActiveCamera());
    mCamera = std::make_shared<Canavar::Engine::DummyCamera>();

    QtImGui::initialize(mWindow);

    mWindow->resize(900, 900);
    mWindow->show();
    mActiveCamera->SetPosition(0, 0, 10);
}

void RangeCalculator::RangeCalculator::Resize(int w, int h)
{
    mCamera->Resize(w, h);

    if (mFramebuffer)
    {
        delete mFramebuffer;
    }

    mFramebuffer = new QOpenGLFramebufferObject(w, h, mFramebufferFormat);
}

void RangeCalculator::RangeCalculator::PostRender(float ifps)
{
    if (mDrawMatches)
    {
        DrawMatches();
    }

    if (mDrawRangeLabel)
    {
        DrawRangeLabel();
    }

    QtImGui::newFrame();
    ImGui::SetNextWindowSize(ImVec2(420, 420), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug");

    ImGui::Text("Width: %d", mCamera->GetWidth());
    ImGui::Text("Height: %d", mCamera->GetHeight());
    ImGui::Text("Camera Position: %.2f, %.2f, %.2f", mActiveCamera->GetPosition().x(), mActiveCamera->GetPosition().y(), mActiveCamera->GetPosition().z());
    ImGui::Text("Distance To Origin: %.2f", mActiveCamera->GetPosition().length());
    ImGui::Text("Delta Angle: %.2f", mDeltaAngle);
    ImGui::Text("H-FOV: %.2f", mCamera->GetHorizontalFov());
    ImGui::SliderFloat("V-FOV", &mActiveCamera->GetVerticalFov_NonConst(), 1.0f, 120.0f);
    ImGui::DragFloat("Dummy Camera Heading Angle", &mDummyCameraHeadingAngle, 0.0025f);
    ImGui::DragFloat("Sensor Size", &mSensorSize, 0.25f);

    ImGui::Checkbox("Undistort Frames", &mUndistortFrames);

    if (ImGui::SliderFloat("Distance Btw. Cameras", &mDistanceBetweenCameras, 0.001f, 20.0f))
    {
        mExtractFeatures = true;
    }

    if (ImGui::SliderFloat("Good Match Ratio", &mGoodMatchRatio, 0.0f, 1.0f))
    {
        mExtractFeatures = true;
    }

    if (ImGui::Checkbox("Cross Check", &mCrossCheck))
    {
        mExtractFeatures = true;
    }

    if (ImGui::Checkbox("Draw Range Label", &mDrawRangeLabel))
    {
        mExtractFeatures = true;
    }

    if (ImGui::Checkbox("Draw Matches", &mDrawMatches))
    {
        mExtractFeatures = true;
    }

    if (ImGui::Button("Calculate Range"))
    {
        mExtractFeatures = true;
    }

    if (ImGui::Button("Save Frame"))
    {
        const auto &[image0, image1] = RenderScene();
        image0.save(QString("Frame%1.png").arg(mSavedFrameCount++));
    }

    ImGui::Text("Avg. Pixel Delta: %.3f", mAveragePixelDelta);
    ImGui::Text("Disparity: %.3f", mDisparity);
    ImGui::Text("Actual Range: %.3f", mActualRange);
    ImGui::Text("Calculated Range: %.3f", mCalculatedRange);
    ImGui::Text("Error Rate: %.3f", mErrorRate);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
    ImGui::Render();
    QtImGui::render();

    if (mExtractFeatures)
    {
        mExtractFeatures = false;

        const auto &[image0, image1] = RenderScene();
        ExtractFeatures(image0, image1);
        CalculateAveragePixelDelta();
        CalculateRangeAndErrorRate();
    }
}

std::pair<QImage, QImage> RangeCalculator::RangeCalculator::RenderScene()
{
    const auto &position = mActiveCamera->GetPosition();
    const auto &rotation = mActiveCamera->GetRotation();
    mCamera->SetPosition(position + rotation * QVector3D(-mDistanceBetweenCameras, 0, 0));
    mCamera->SetRotation(rotation * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), mDummyCameraHeadingAngle));
    mCamera->SetVerticalFov(mActiveCamera->GetVerticalFov());

    mRenderingManager->RenderToFramebuffer(mFramebuffer, mCamera.get());

    const auto fbo = mRenderingManager->GetFramebuffer(Canavar::Engine::Framebuffer::Singlesample);
    auto image0 = fbo->toImage(true, 0); // Take color attachment
    image0 = image0.convertedTo(QImage::Format_Grayscale8);

    auto image1 = mFramebuffer->toImage(true, 0); // Take color attachment
    image1 = image1.convertedTo(QImage::Format_Grayscale8);

    return { image0, image1 };
}

void RangeCalculator::RangeCalculator::ExtractFeatures(const QImage &image0, const QImage &image1)
{
    mExtractor = cv::ORB::create();
    mMatcher = cv::BFMatcher::create(cv::NORM_HAMMING, mCrossCheck);

    mKeyPoints[Camera_0].clear();
    mKeyPoints[Camera_1].clear();

    mGoodKeyPoints[Camera_0].clear();
    mGoodKeyPoints[Camera_1].clear();

    mAllMatches.clear();
    mGoodMatches.clear();

    cv::Mat mat[2];

    mat[Camera_0] = cv::Mat(image0.height(), image0.width(), CV_8U, (void *) image0.bits(), image0.bytesPerLine());
    mat[Camera_1] = cv::Mat(image1.height(), image1.width(), CV_8U, (void *) image1.bits(), image1.bytesPerLine());

    try
    {
        if (mUndistortFrames)
        {
            mat[Camera_0] = Undistort(mat[Camera_0]);
            mat[Camera_1] = Undistort(mat[Camera_1]);
        }

        cv::Mat desc[2];

        desc[Camera_0] = cv::Mat(mat[Camera_0].rows, mat[Camera_0].cols, CV_8U);
        desc[Camera_1] = cv::Mat(mat[Camera_1].rows, mat[Camera_1].cols, CV_8U);

        mExtractor->detect(mat[Camera_0], mKeyPoints[Camera_0]);
        mExtractor->detect(mat[Camera_1], mKeyPoints[Camera_1]);

        mExtractor->compute(mat[Camera_0], mKeyPoints[Camera_0], desc[Camera_0]);
        mExtractor->compute(mat[Camera_1], mKeyPoints[Camera_1], desc[Camera_1]);

        mMatcher->match(desc[Camera_0], desc[Camera_1], mAllMatches);
    }
    catch (const std::exception &e)
    {
        qFatal() << e.what();
    }

    // Sort good matches to bad
    std::sort(mAllMatches.begin(), mAllMatches.end(), [](const cv::DMatch &m0, const cv::DMatch &m1) { return m0.distance < m1.distance; });

    int nGoodMatches = mGoodMatchRatio * mAllMatches.size();

    for (int i = 0; i < nGoodMatches; ++i)
    {
        const auto &match = mAllMatches[i];

        mGoodKeyPoints[Camera_0].push_back(mKeyPoints[Camera_0][match.queryIdx].pt);
        mGoodKeyPoints[Camera_1].push_back(mKeyPoints[Camera_1][match.trainIdx].pt);
        mGoodMatches.push_back(match);
    }
}

void RangeCalculator::RangeCalculator::CalculateAveragePixelDelta()
{
    float totalDistance = 0.0f;

    for (const auto &match : mGoodMatches)
    {
        const auto &kp0 = mKeyPoints[Camera_0][match.queryIdx];
        const auto &kp1 = mKeyPoints[Camera_1][match.trainIdx];

        float dx = kp0.pt.x - kp1.pt.x;
        float dy = kp0.pt.y - kp1.pt.y;

        totalDistance += std::sqrt(dx * dx + dy * dy);
    }

    mAveragePixelDelta = totalDistance / mGoodMatches.size();

    mDeltaAngle = mAveragePixelDelta / mActiveCamera->GetWidth();
}

void RangeCalculator::RangeCalculator::CalculateRangeAndErrorRate()
{
    // const auto &bestMatch = mGoodMatches[0];
    // const auto &kp0 = mKeyPoints[Camera_0][bestMatch.queryIdx];
    // const auto &kp1 = mKeyPoints[Camera_1][bestMatch.trainIdx];

    // QVector2D p0 = Undistort(kp0.pt.x, kp0.pt.y);
    // QVector2D p1 = Undistort(kp1.pt.x, kp1.pt.y);

    mDisparity = mAveragePixelDelta;

    // // f = s / (2 * tan(FOV/2)) where
    // // f is focal length
    // // s is sensor size
    // // FOV is horizontal field of view in radians

    mFocalLength = mSensorSize / (2 * std::tan(qDegreesToRadians(mActiveCamera->GetHorizontalFov() / 2.0f)));

    // // Z = f * B / d where
    // // Z is depth value
    // // f is focal length
    // // B is baseline
    // // d is disparity

    mCalculatedRange = mDistanceBetweenCameras * mFocalLength / mDisparity;

    mActualRange = mActiveCamera->GetPosition().length();

    mErrorRate = 100.0f * (1 - mCalculatedRange / mActualRange);
}

void RangeCalculator::RangeCalculator::DrawMatches()
{
    QPainter painter(mWindow);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

    painter.setPen(QColor(0, 255, 255));

    for (const auto &match : mGoodMatches)
    {
        const auto &kp0 = mKeyPoints[Camera_0][match.queryIdx];
        const auto &kp1 = mKeyPoints[Camera_1][match.trainIdx];
        constexpr float R = 5;
        painter.drawEllipse(kp0.pt.x - R, kp0.pt.y - R, 2 * R, 2 * R);
        painter.drawEllipse(kp1.pt.x - R, kp1.pt.y - R, 2 * R, 2 * R);
        painter.drawLine(kp0.pt.x, kp0.pt.y, kp1.pt.x, kp1.pt.y);
    }
}

void RangeCalculator::RangeCalculator::DrawRangeLabel()
{
    if (mGoodMatches.size() == 0)
    {
        return;
    }

    QPainter painter(mWindow);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

    const auto &bestMatch = mGoodMatches[0];
    const auto &kp = mKeyPoints[Camera_0][bestMatch.queryIdx];

    QFont font("Arial", 20, QFont::Bold);
    QPen pen(QColor(255, 0, 0), 2);

    painter.setPen(pen);
    painter.setFont(font);

    constexpr float R = 5;
    painter.drawEllipse(kp.pt.x - R, kp.pt.y - R, 2 * R, 2 * R);

    painter.drawLine(kp.pt.x, kp.pt.y, kp.pt.x + 45, kp.pt.y - 45);
    painter.drawText(kp.pt.x + 50, kp.pt.y - 50, QString("%1 units").arg(mCalculatedRange));
    painter.drawText(kp.pt.x + 50, kp.pt.y - 15, QString("~%1%").arg(mErrorRate));
}

cv::Mat RangeCalculator::RangeCalculator::Undistort(cv::Mat image)
{
    cv::Mat_<double> cameraMatrix(3, 3); // 3x3 matrix
    cv::Mat_<double> distCoeffs(5, 1);   // 5x1 matrix for five distortion coefficients

    // cameraMatrix << 450, 0, 450, 0, 450, 450, 0, 0, 1;
    // distCoeffs << -0.001530735984887822, 6.558364577199068e-05, -0.001383235185926778, 0.001275768542539859, 7.168408175765235e-06;

    cv::Mat undistorted;
    cv::undistort(image, undistorted, mCameraCalibration.GetCameraMatrix(), mCameraCalibration.GetDistCoeffs());

    // cv::imwrite("Undistored.png", undistorted);

    return undistorted;
}
