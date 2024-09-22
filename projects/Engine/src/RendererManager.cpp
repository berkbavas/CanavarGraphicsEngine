#include "RendererManager.h"
#include "CameraManager.h"
#include "Config.h"
#include "FirecrackerEffect.h"
#include "Haze.h"
#include "Helper.h"
#include "LightManager.h"
#include "Model.h"
#include "ModelDataManager.h"
#include "NodeManager.h"
#include "NozzleEffect.h"
#include "PointLight.h"
#include "ShaderManager.h"
#include "Sky.h"
#include "Sun.h"
#include "Terrain.h"
#include "LightningStrikeGenerator.h"

#include <QDir>

Canavar::Engine::RendererManager::RendererManager()
    : Manager()
    , mWidth(1600)
    , mHeight(900)
    , mBlurPass(4)
    , mExposure(1.0f)
    , mGamma(1.0f)
    , mColorAttachments{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 }
{}

Canavar::Engine::RendererManager* Canavar::Engine::RendererManager::Instance()
{
    static RendererManager instance;
    return &instance;
}

bool Canavar::Engine::RendererManager::Init()
{
    mConfig = Config::Instance();
    mNodeManager = NodeManager::Instance();
    mCameraManager = CameraManager::Instance();
    mLightManager = LightManager::Instance();
    mShaderManager = ShaderManager::Instance();
    mModelDataManager = ModelDataManager::Instance();

    mSky = Sky::Instance();
    mSun = Sun::Instance();
    mHaze = Haze::Instance();
    mTerrain = Terrain::Instance();

    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_BLEND);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.5f);

    // Default FBO format
    mFBOFormats.insert(FramebufferType::Default, new QOpenGLFramebufferObjectFormat);
    mFBOFormats[FramebufferType::Default]->setSamples(4);
    mFBOFormats[FramebufferType::Default]->setAttachment(QOpenGLFramebufferObject::Depth);
    mFBOFormats[FramebufferType::Default]->setInternalTextureFormat(GL_RGBA32F);

    // Other formats
    mFBOFormats.insert(FramebufferType::Temporary, new QOpenGLFramebufferObjectFormat);
    mFBOFormats.insert(FramebufferType::Ping, new QOpenGLFramebufferObjectFormat);
    mFBOFormats.insert(FramebufferType::Pong, new QOpenGLFramebufferObjectFormat);

    CreateFramebuffers(mWidth, mHeight);

    // Quad
    glGenVertexArrays(1, &mQuad.mVAO);
    glBindVertexArray(mQuad.mVAO);
    glGenBuffers(1, &mQuad.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mQuad.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::QUAD), Canavar::Engine::QUAD, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(QVector2D), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(QVector2D), (void*)sizeof(QVector2D));

    // Cube
    glGenVertexArrays(1, &mCube.mVAO);
    glBindVertexArray(mCube.mVAO);
    glGenBuffers(1, &mCube.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mCube.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE), CUBE, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void*)0);
    glEnableVertexAttribArray(0);

    // Cube Strip
    glGenVertexArrays(1, &mCubeStrip.mVAO);
    glBindVertexArray(mCubeStrip.mVAO);
    glGenBuffers(1, &mCubeStrip.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mCubeStrip.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE_STRIP), CUBE_STRIP, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void*)0);
    glEnableVertexAttribArray(0);

    // Line Strip
    glGenVertexArrays(1, &mLineStripHandle.mVAO);
    glBindVertexArray(mLineStripHandle.mVAO);
    glGenBuffers(1, &mLineStripHandle.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mLineStripHandle.mVBO);
    glBufferData(GL_ARRAY_BUFFER, 64 * sizeof(QVector3D), nullptr, GL_DYNAMIC_COPY);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void*)0);
    glEnableVertexAttribArray(0);

    return true;
}

void Canavar::Engine::RendererManager::Resize(int w, int h)
{
    mWidth = w;
    mHeight = h;

    DeleteFramebuffers();
    CreateFramebuffers(mWidth, mHeight);
}

void Canavar::Engine::RendererManager::Render(float ifps)
{
    mCamera = mCameraManager->GetActiveCamera();
    mClosePointLights = Helper::GetClosePointLights(mLightManager->GetPointLights(), mCamera->WorldPosition(), 8);

    // Common uniforms
    mShaderManager->Bind(ShaderType::ModelColoredShader);
    SetCommonUniforms();
    mShaderManager->Release();

    mShaderManager->Bind(ShaderType::ModelTexturedShader);
    SetCommonUniforms();
    mShaderManager->Release();

    mShaderManager->Bind(ShaderType::TerrainShader);
    SetCommonUniforms();
    mShaderManager->Release();

    mFBOs[FramebufferType::Default]->bind();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render Sky
    mSky->Render();

    // Render terrain
    mTerrain->Render();

    const auto& nodes = mNodeManager->GetNodes();

    // Render Models
    for (const auto& node : nodes)
    {
        if (!node->GetVisible())
            continue;

        if (auto model = dynamic_cast<Model*>(node))
            model->Render(RenderMode::Default);
    }

    // Render Effects
    for (const auto& node : nodes)
    {
        if (!node->GetVisible())
            continue;

        if (auto effect = dynamic_cast<NozzleEffect*>(node))
            effect->Render(ifps);

        if (auto effect = dynamic_cast<FirecrackerEffect*>(node))
            effect->Render(ifps);
    }

    for (const auto& node : nodes)
    {
        if (auto lsg = dynamic_cast<LightningStrikeBase*>(node))
        {
            lsg->Render(ifps);
        }
    }

    // Selectables
    if (mConfig->GetNodeSelectionEnabled())
    {
        const auto& VP = mCamera->GetViewProjectionMatrix();
        const auto& nodes = mSelectableNodes.keys();
        const auto& models = mSelectedMeshes.keys();

        mShaderManager->Bind(ShaderType::BasicShader);

        for (const auto& node : nodes)
        {
            mShaderManager->SetUniformValue("MVP", VP * node->WorldTransformation() * node->GetAABB().GetTransformation());

            mShaderManager->SetUniformValue("color", mSelectableNodes.value(node, QVector4D(1, 1, 1, 1)));
            glBindVertexArray(mCubeStrip.mVAO);
            glDrawArrays(GL_LINE_STRIP, 0, 17);
        }

        for (const auto& model : models)
        {
            const auto& parameters = mSelectedMeshes.value(model);
            const auto& meshTransformation = model->GetMeshTransformation(parameters.mMesh->GetName());
            mShaderManager->SetUniformValue("MVP", VP * model->WorldTransformation() * meshTransformation * parameters.mMesh->GetAABB().GetTransformation());
            mShaderManager->SetUniformValue("color", parameters.mMeshStripColor);
            glBindVertexArray(mCubeStrip.mVAO);
            glDrawArrays(GL_LINE_STRIP, 0, 17);
        }

        mShaderManager->Release();

        // Vertices of Mesh
        mShaderManager->Bind(ShaderType::MeshVertexRendererShader);

        for (const auto& model : models)
        {
            const auto& parameters = mSelectedMeshes.value(model);

            if (parameters.mRenderVertices)
            {
                mShaderManager->SetUniformValue("MVP", VP * model->WorldTransformation() * model->GetMeshTransformation(parameters.mMesh->GetName()));
                mShaderManager->SetUniformValue("scale", parameters.mScale);
                mShaderManager->SetUniformValue("selectedVertexID", parameters.mSelectedVertexID);
                mShaderManager->SetUniformValue("vertexColor", parameters.mVertexColor);
                mShaderManager->SetUniformValue("selectedVertexColor", parameters.mSelectedVertexColor);

                parameters.mMesh->GetVerticesVAO()->bind();
                glDrawArraysInstanced(GL_TRIANGLES, 0, 36, parameters.mMesh->GetNumberOfVertices());
                parameters.mMesh->GetVerticesVAO()->release();
            }
        }

        mShaderManager->Release();
    }

    // Line Strip
    {
        mShaderManager->Bind(ShaderType::LineStripShader);
        mShaderManager->SetUniformValue("VP", mCamera->GetViewProjectionMatrix());
        glBindVertexArray(mLineStripHandle.mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mLineStripHandle.mVBO);

        for (const auto& lineStrip : mLineStrips)
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0, lineStrip->GetPoints().size() * sizeof(QVector3D), lineStrip->GetPoints().constData());
            mShaderManager->SetUniformValue("color", lineStrip->GetColor());
            glDrawArrays(GL_LINE_STRIP, 0, lineStrip->GetPoints().size());
        }

        mShaderManager->Release();
    }

    // Default render pass is done
    mFBOs[FramebufferType::Default]->release();

    // Blur
    QOpenGLFramebufferObject::blitFramebuffer(mFBOs[FramebufferType::Ping], //
        QRect(0, 0, mWidth, mHeight),
        mFBOs[FramebufferType::Default],
        QRect(0, 0, mWidth, mHeight),
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST,
        1,
        0);

    for (int i = 0; i < qMax(0, mBlurPass); i++)
    {
        mFBOs[i % 2 == 0 ? FramebufferType::Pong : FramebufferType::Ping]->bind();
        mShaderManager->Bind(ShaderType::BlurShader);
        mShaderManager->SetUniformValue("horizontal", i % 2 == 0);
        mShaderManager->SetSampler("screenTexture", 0, mFBOs[i % 2 == 0 ? FramebufferType::Ping : FramebufferType::Pong]->texture());
        glBindVertexArray(mQuad.mVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        mShaderManager->Release();
    }

    // Post process (combine blur and scene)
    QOpenGLFramebufferObject::blitFramebuffer(mFBOs[FramebufferType::Temporary], mFBOs[FramebufferType::Default]);

    QOpenGLFramebufferObject::bindDefault();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mShaderManager->Bind(ShaderType::PostProcessShader);
    mShaderManager->SetSampler("sceneTexture", 0, mFBOs[FramebufferType::Temporary]->texture());
    mShaderManager->SetSampler("bloomBlurTexture", 1, mFBOs[qMax(0, mBlurPass) % 2 == 0 ? FramebufferType::Ping : FramebufferType::Pong]->texture());
    mShaderManager->SetUniformValue("exposure", mExposure);
    mShaderManager->SetUniformValue("gamma", mGamma);
    glBindVertexArray(mQuad.mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    mShaderManager->Release();
}

void Canavar::Engine::RendererManager::SetCommonUniforms()
{
    mShaderManager->SetUniformValue("cameraPos", mCamera->WorldPosition());
    mShaderManager->SetUniformValue("VP", mCamera->GetViewProjectionMatrix());

    mShaderManager->SetUniformValue("sun.direction", -mSun->GetDirection().normalized());
    mShaderManager->SetUniformValue("sun.color", mSun->GetColor());
    mShaderManager->SetUniformValue("sun.ambient", mSun->GetAmbient());
    mShaderManager->SetUniformValue("sun.diffuse", mSun->GetDiffuse());
    mShaderManager->SetUniformValue("sun.specular", mSun->GetSpecular());

    mShaderManager->SetUniformValue("haze.enabled", mHaze->GetEnabled());
    mShaderManager->SetUniformValue("haze.color", mHaze->GetColor());
    mShaderManager->SetUniformValue("haze.density", mHaze->GetDensity());
    mShaderManager->SetUniformValue("haze.gradient", mHaze->GetGradient());

    mShaderManager->SetUniformValue("numberOfPointLights", (int)mClosePointLights.size());

    for (int i = 0; i < mClosePointLights.size(); i++)
    {
        mShaderManager->SetUniformValue("pointLights[" + QString::number(i) + "].color", mClosePointLights[i]->GetColor());
        mShaderManager->SetUniformValue("pointLights[" + QString::number(i) + "].position", mClosePointLights[i]->WorldPosition());
        mShaderManager->SetUniformValue("pointLights[" + QString::number(i) + "].ambient", mClosePointLights[i]->GetAmbient());
        mShaderManager->SetUniformValue("pointLights[" + QString::number(i) + "].diffuse", mClosePointLights[i]->GetDiffuse());
        mShaderManager->SetUniformValue("pointLights[" + QString::number(i) + "].specular", mClosePointLights[i]->GetSpecular());
        mShaderManager->SetUniformValue("pointLights[" + QString::number(i) + "].constant", mClosePointLights[i]->GetConstant());
        mShaderManager->SetUniformValue("pointLights[" + QString::number(i) + "].linear", mClosePointLights[i]->GetLinear());
        mShaderManager->SetUniformValue("pointLights[" + QString::number(i) + "].quadratic", mClosePointLights[i]->GetQuadratic());
    }
}

void Canavar::Engine::RendererManager::DeleteFramebuffers()
{
    auto keys = mFBOs.keys();

    for (auto type : keys)
        if (mFBOs[type])
            delete mFBOs[type];
}

void Canavar::Engine::RendererManager::CreateFramebuffers(int width, int height)
{
    auto keys = mFBOFormats.keys();

    for (auto type : keys)
    {
        mFBOs.insert(type, new QOpenGLFramebufferObject(width, height, *mFBOFormats[type]));

        if (type == FramebufferType::Default)
        {
            mFBOs[FramebufferType::Default]->addColorAttachment(width, height, GL_RGBA32F);

            mFBOs[FramebufferType::Default]->bind();
            glDrawBuffers(2, mColorAttachments);
            mFBOs[FramebufferType::Default]->release();
        }
    }
}

void Canavar::Engine::RendererManager::OnSelectedNodeDestroyed(QObject* node)
{
    mSelectableNodes.remove(static_cast<Node*>(node));
}

void Canavar::Engine::RendererManager::OnSelectedModelDestroyed(QObject* model)
{
    mSelectedMeshes.remove(static_cast<Model*>(model));
}

void Canavar::Engine::RendererManager::OnLineStripDestroyed(QObject* lineStrip)
{
    mLineStrips.removeAll(static_cast<LineStrip*>(lineStrip));
}

void Canavar::Engine::RendererManager::AddSelectableNode(Node* node, QVector4D color)
{
    if (node && node->GetSelectable())
    {
        mSelectableNodes.insert(node, color);
        connect(node, &QObject::destroyed, this, &RendererManager::OnSelectedNodeDestroyed, Qt::QueuedConnection);
    }
}

void Canavar::Engine::RendererManager::RemoveSelectableNode(Node* node)
{
    if (node)
    {
        mSelectableNodes.remove(node);
        disconnect(node, &QObject::destroyed, this, &RendererManager::OnSelectedNodeDestroyed);
    }
}

void Canavar::Engine::RendererManager::AddSelectedMesh(Model* model, const SelectedMeshParameters& parameters)
{
    if (model)
    {
        mSelectedMeshes.insert(model, parameters);
        connect(model, &QObject::destroyed, this, &RendererManager::OnSelectedModelDestroyed, Qt::QueuedConnection);
    }
}

void Canavar::Engine::RendererManager::RemoveSelectedMesh(Model* model)
{
    if (model)
    {
        mSelectedMeshes.remove(model);
        disconnect(model, &QObject::destroyed, this, &RendererManager::OnSelectedModelDestroyed);
    }
}

void Canavar::Engine::RendererManager::AddLineStrip(LineStrip* lineStrip)
{
    if (lineStrip)
    {
        mLineStrips << lineStrip;
        connect(lineStrip, &QObject::destroyed, this, &RendererManager::OnLineStripDestroyed, Qt::QueuedConnection);
    }
}

void Canavar::Engine::RendererManager::RemoveLineStrip(LineStrip* lineStrip)
{
    if (lineStrip)
    {
        mLineStrips.removeAll(lineStrip);
        disconnect(lineStrip, &QObject::destroyed, this, &RendererManager::OnLineStripDestroyed);
    }
}

Canavar::Engine::SelectedMeshParameters Canavar::Engine::RendererManager::GetSelectedMeshParameters(Model* model) const
{
    return mSelectedMeshes.value(model);
}

Canavar::Engine::SelectedMeshParameters& Canavar::Engine::RendererManager::GetSelectedMeshParameters_Ref(Model* model)
{
    return mSelectedMeshes[model];
}

const QMap<Canavar::Engine::Model*, Canavar::Engine::SelectedMeshParameters>& Canavar::Engine::RendererManager::GetSelectedMeshes() const
{
    return mSelectedMeshes;
}