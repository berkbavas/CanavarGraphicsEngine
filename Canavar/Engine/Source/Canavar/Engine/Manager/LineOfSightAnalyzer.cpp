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

    QOpenGLFramebufferObjectFormat Format;
    Format.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    Format.setSamples(0);

    mDebugFramebuffer = std::make_unique<Framebuffer>(512, 512, Format);

    CreateObserverCameras();

    mObserverSphere = mRenderer->GetNodeManager()->CreateNode<Sphere>();
    mObserverSphere->SetVisible(mEnabled);
    mObserverSphere->SetScale(10.0f);
}

void Canavar::Engine::LineOfSightAnalyzer::Update(float)
{
    if (mRenderDebug)
    {
        RenderDebugFaceInner(mLosDebugFaceIndex);
        mRenderDebug = false;
    }

    if (!ShouldRender())
    {
        return;
    }

    OpenGLStateGuard StateGuard(this);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

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
    mShader->SetUniform("uObserverPosition", GetObserverPosition());
    mShader->SetUniform("uFarPlane", mMaxLosDistance);
    mShader->SetUniform("uWidth", mTerrain->GetWidth());
    mShader->SetUniform("uCameraPosition", GetObserverPosition());
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

QVector3D Canavar::Engine::LineOfSightAnalyzer::GetObserverPosition() const
{
    return mObserverPositionOnTerrain + QVector3D(0, mObserverHeightOnTerrain, 0);
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
    OpenGLStateGuard StateGuard(this);

    mDebugFramebuffer->Bind();
    glViewport(0, 0, mDebugFramebuffer->GetWidth(), mDebugFramebuffer->GetHeight());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    mDebugShader->Bind();
    mDebugShader->SetSampler("uDepthMap", 0, GetDepthMap(), GL_TEXTURE_CUBE_MAP);
    mDebugShader->SetUniform("uFaceIndex", FaceIndex);
    mDebugQuad->Render();
    mDebugShader->Unbind();
    mDebugFramebuffer->Unbind();
}

void Canavar::Engine::LineOfSightAnalyzer::SetObserverPositionOnTerrain(const QVector3D &Position)
{
    mObserverPositionOnTerrain = Position;
    mIsDirty = true;
}

void Canavar::Engine::LineOfSightAnalyzer::SetObserverHeightOnTerrain(float Height)
{
    mObserverHeightOnTerrain = Height;
    mIsDirty = true;
}

void Canavar::Engine::LineOfSightAnalyzer::SetMaxLosDistance(float Distance)
{
    mMaxLosDistance = Distance;
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

    mObserverSphere->SetVisible(mEnabled);
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

void Canavar::Engine::LineOfSightAnalyzer::UpdateObserverCameras()
{
    for (int i = 0; i < mObserverCameras.size(); i++)
    {
        mObserverCameras.at(i)->SetPosition(GetObserverPosition());
        mObserverCameras.at(i)->SetZNear(1.0f);
        mObserverCameras.at(i)->SetZFar(mMaxLosDistance);
    }

    mObserverSphere->SetPosition(GetObserverPosition());
}

void Canavar::Engine::LineOfSightAnalyzer::CreateObserverCameras()
{
    for (int i = 0; i < 6; i++)
    {
        mObserverCameras.push_back(std::make_unique<DummyCamera>());
        mObserverCameras.at(i)->Resize(mWidth, mHeight);
        mObserverCameras.at(i)->SetVerticalFov(90.0f);
        mObserverCameras.at(i)->SetZNear(1.0f);
        mObserverCameras.at(i)->SetZFar(mMaxLosDistance);
        mObserverCameras.at(i)->SetPosition(GetObserverPosition());
    }

    // Rotations must match OpenGL cubemap sampling convention (up=-Y for ±X, ±Z faces).
    mObserverCameras.at(0)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 180.0f) * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), -90.0f)); // +X
    mObserverCameras.at(1)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 180.0f) * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 90.0f));  // -X
    mObserverCameras.at(2)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90.0f));                                                              // +Y
    mObserverCameras.at(3)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90.0f));                                                             // -Y
    mObserverCameras.at(4)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 180.0f));                                                             // +Z
    mObserverCameras.at(5)->SetRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), 180.0f));                                                             // -Z
}

bool Canavar::Engine::LineOfSightAnalyzer::ShouldRender() const
{
    return mIsDirty && mTerrain && mEnabled;
}
