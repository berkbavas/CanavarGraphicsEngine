#include "LineOfSightAnalyzer.h"

#include "Canavar/Engine/Core/OpenGLStateGuard.h"
#include "Canavar/Engine/GlobalNode/Terrain/Terrain.h"
#include "Canavar/Engine/Manager/Renderer.h"
#include "Canavar/Engine/Model/PrimitiveModel/Primitives.h"
#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::LineOfSightAnalyzer::LineOfSightAnalyzer(Renderer *pRenderer)
    : mRenderer(pRenderer)
{}

void Canavar::Engine::LineOfSightAnalyzer::Initialize()
{
    initializeOpenGLFunctions();

    mFramebuffer = std::make_unique<CubicFramebuffer>(mWidth, mHeight);

    mShader = std::make_unique<Shader>("Line Of Sight Analyzer Shader");
    mShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tesc");
    mShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tese");
    mShader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/LineOfSightAnalyzer.geom");
    mShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/LineOfSightAnalyzer.frag");
    mShader->Initialize();

    mDebugShader = std::make_unique<Shader>("Line Of Sight Analyzer Debug Shader");
    mDebugShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mDebugShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/LineOfSightAnalyzerDebug.frag");
    mDebugShader->Initialize();

    mDebugQuad = std::make_unique<Quad>();

    glGenFramebuffers(1, &mDebugFBO);
    glGenTextures(1, &mDebugTexture);
    glBindTexture(GL_TEXTURE_2D, mDebugTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mDebugTextureSize, mDebugTextureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, mDebugFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mDebugTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        CGE_EXIT_FAILURE("LineOfSightAnalyzer: Debug framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CreateObserverCameras();

    mObserverSphere = mRenderer->GetNodeManager()->CreateNode<Sphere>();
    mObserverSphere->SetScale(10.0f);
}

void Canavar::Engine::LineOfSightAnalyzer::Update(float)
{
    OpenGLStateGuard StateGuard(this);

    if (mRenderDebug)
    {
        RenderDebugFaceInner(mLosDebugFaceIndex);
        mRenderDebug = false;
    }

    if (!ShouldRender())
    {
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    UpdateObserverCameras();

    mFramebuffer->Clear();
    mFramebuffer->Bind();
    glViewport(0, 0, mFramebuffer->GetWidth(), mFramebuffer->GetHeight());

    mShader->Bind();

    for (int i = 0; i < mObserverCameras.size(); i++)
    {
        mShader->SetUniform(QString("uVPs[%1]").arg(i), mObserverCameras.at(i)->GetViewProjectionMatrix());
    }

    mShader->SetUniform("uLogarithmicDepthBuffer", 0); // Disable logarithmic depth buffer for line of sight analysis
    mShader->SetUniform("uObserverPosition", mObserverCameras.at(0)->GetPosition());
    mShader->SetUniform("uFarPlane", mObserverCameras.at(0)->GetZFar());
    mShader->SetUniform("uWidth", mTerrain->GetWidth());
    mShader->SetUniform("uCameraPosition", mRenderer->GetActiveCamera()->GetWorldPosition());
    mShader->SetUniform("uTessellationMultiplier", mTerrain->GetTessellationMultiplier());
    mShader->SetUniform("uEarthRadius", static_cast<float>(Wgs84::SemiMajorAxis));
    mShader->SetUniform("uNoise.Octaves", mTerrain->GetOctaves());
    mShader->SetUniform("uNoise.Amplitude", mTerrain->GetAmplitude());
    mShader->SetUniform("uNoise.Frequency", mTerrain->GetFrequency());
    mShader->SetUniform("uNoise.Persistence", mTerrain->GetPersistence());
    mShader->SetUniform("uNoise.Lacunarity", mTerrain->GetLacunarity());
    mTerrain->RenderPatches();
    mShader->Unbind();
    mFramebuffer->Unbind();

    mIsDirty = false;
}

void Canavar::Engine::LineOfSightAnalyzer::SetTerrain(Terrain *pTerrain)
{
    mTerrain = pTerrain;
    mIsDirty = true;
}

float Canavar::Engine::LineOfSightAnalyzer::GetFarPlane() const
{
    return mObserverCameras.empty() ? 0.0f : mObserverCameras.at(0)->GetZFar();
}

GLuint Canavar::Engine::LineOfSightAnalyzer::GetDepthMap() const
{
    return mFramebuffer ? mFramebuffer->GetDepthMap() : GLuint();
}

void Canavar::Engine::LineOfSightAnalyzer::RenderDebugFace(int FaceIndex)
{
    mLosDebugFaceIndex = FaceIndex;
    mRenderDebug = true;
}

void Canavar::Engine::LineOfSightAnalyzer::RenderDebugFaceInner(int FaceIndex)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mDebugFBO);
    glViewport(0, 0, mDebugTextureSize, mDebugTextureSize);
    glDisable(GL_DEPTH_TEST);

    mDebugShader->Bind();
    mDebugShader->SetSampler("uDepthMap", 0, GetDepthMap(), GL_TEXTURE_CUBE_MAP);
    mDebugShader->SetUniform("uFaceIndex", FaceIndex);
    mDebugQuad->Render();
    mDebugShader->Unbind();

    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canavar::Engine::LineOfSightAnalyzer::SetMinLosDistance(float Distance)
{
    mMinLosDistance = Distance;
    mIsDirty = true;
}

void Canavar::Engine::LineOfSightAnalyzer::SetMaxLosDistance(float Distance)
{
    mMaxLosDistance = Distance;
    mIsDirty = true;
}

void Canavar::Engine::LineOfSightAnalyzer::SetObserverPosition(const QVector3D &Position)
{
    mObserverPosition = Position;
    mIsDirty = true;
}

void Canavar::Engine::LineOfSightAnalyzer::SetBias(float Bias)
{
    mBias = Bias;
    mIsDirty = true;
}

void Canavar::Engine::LineOfSightAnalyzer::SetEnabled(bool Enabled)
{
    mEnabled = Enabled;
    mIsDirty = true;

    mEnabled ? mObserverSphere->SetVisible(true) : mObserverSphere->SetVisible(false);
}

void Canavar::Engine::LineOfSightAnalyzer::SetVisibilityOpacity(float Opacity)
{
    mVisibilityOpacity = Opacity;
    mIsDirty = true;
}

void Canavar::Engine::LineOfSightAnalyzer::SetShadowOpacity(float Opacity)
{
    mShadowOpacity = Opacity;
}

void Canavar::Engine::LineOfSightAnalyzer::SetShadowColor(const QVector3D &Color)
{
    mShadowColor = Color;
}

void Canavar::Engine::LineOfSightAnalyzer::SetObserverHeight(float Height)
{
    mObserverHeight = Height;
    mIsDirty = true;
}

void Canavar::Engine::LineOfSightAnalyzer::UpdateObserverCameras()
{
    const auto ObserverPositionWithHeight = mObserverPosition + QVector3D(0, mObserverHeight, 0);

    for (int i = 0; i < mObserverCameras.size(); i++)
    {
        mObserverCameras.at(i)->SetPosition(ObserverPositionWithHeight);
        mObserverCameras.at(i)->SetZNear(mMinLosDistance);
        mObserverCameras.at(i)->SetZFar(mMaxLosDistance);
    }

    mObserverSphere->SetPosition(ObserverPositionWithHeight);
}

void Canavar::Engine::LineOfSightAnalyzer::CreateObserverCameras()
{
    for (int i = 0; i < 6; i++)
    {
        mObserverCameras.push_back(std::make_unique<FreeCamera>());
        mObserverCameras.at(i)->Resize(mWidth, mHeight);
        mObserverCameras.at(i)->SetVerticalFov(90.0f);
        mObserverCameras.at(i)->SetZNear(mMinLosDistance);
        mObserverCameras.at(i)->SetZFar(mMaxLosDistance);
    }

    const auto RollFix = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), 180);

    mObserverCameras.at(0)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), -90) * RollFix);
    mObserverCameras.at(1)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 90) * RollFix);
    mObserverCameras.at(2)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90));
    mObserverCameras.at(3)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    mObserverCameras.at(4)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180) * RollFix);
    mObserverCameras.at(5)->SetRotation(RollFix);
}

bool Canavar::Engine::LineOfSightAnalyzer::ShouldRender() const
{
    return mIsDirty && mTerrain && mEnabled;
}
