#include "SelectableNodeRenderer.h"

#include "CameraManager.h"
#include "Config.h"
#include "Mesh.h"
#include "Model.h"
#include "NodeManager.h"
#include "RendererManager.h"
#include "ShaderManager.h"

#include <QTimer>

Canavar::Engine::SelectableNodeRenderer::SelectableNodeRenderer()
    : Manager()
    , mWidth(1600)
    , mHeight(900)
    , mResizeLater(true)
{}

bool Canavar::Engine::SelectableNodeRenderer::Init()
{
    mConfig = Config::Instance();
    mShaderManager = ShaderManager::Instance();
    mNodeManager = NodeManager::Instance();
    mCameraManager = CameraManager::Instance();
    mRendererManager = RendererManager::Instance();

    initializeOpenGLFunctions();
    mNodeInfoFBO.Init();
    mNodeInfoFBO.Create(mWidth, mHeight);

    glGenVertexArrays(1, &mCube.mVAO);
    glBindVertexArray(mCube.mVAO);
    glGenBuffers(1, &mCube.mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mCube.mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Canavar::Engine::CUBE), CUBE, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return true;
}

void Canavar::Engine::SelectableNodeRenderer::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    if (mConfig->GetNodeSelectionEnabled())
    {
        if (mResizeLater)
        {
            QTimer::singleShot(200, [=]() {
                mNodeInfoFBO.Destroy();
                mNodeInfoFBO.Create(mWidth, mHeight);
                mResizeLater = true;
                });

            mResizeLater = false;
        }
    }
}

Canavar::Engine::SelectableNodeRenderer::NodeInfo Canavar::Engine::SelectableNodeRenderer::GetNodeInfoByScreenPosition(int x, int y)
{
    NodeInfo info;

    if (mConfig->GetNodeSelectionEnabled())
    {
        mNodeInfoFBO.Bind();
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        glReadPixels(x, mHeight - y, 1, 1, GL_RGBA_INTEGER, GL_INT, &info);
        mNodeInfoFBO.Release();
    }

    return info;
}

Canavar::Engine::SelectableNodeRenderer::NodeInfo Canavar::Engine::SelectableNodeRenderer::GetVertexInfoByScreenPosition(int x, int y)
{
    NodeInfo info;

    if (mConfig->GetNodeSelectionEnabled())
    {
        mNodeInfoFBO.Bind();
        glReadBuffer(GL_COLOR_ATTACHMENT1);

        glReadPixels(x, mHeight - y, 1, 1, GL_RGBA_INTEGER, GL_INT, &info);
        mNodeInfoFBO.Release();
    }

    return info;
}

void Canavar::Engine::SelectableNodeRenderer::Render(float)
{
    if (mConfig->GetNodeSelectionEnabled())
    {
        mNodeInfoFBO.Bind();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto& VP = mCameraManager->GetActiveCamera()->GetViewProjectionMatrix();

        for (const auto& node : mNodeManager->GetNodes())
        {
            if (!node->GetVisible() || !node->GetSelectable())
                continue;

            if (auto model = dynamic_cast<Model*>(node))
            {
                model->Render(RenderMode::NodeInfo);

                const auto& params = mRendererManager->GetSelectedMeshParameters(model);

                if (params.mRenderVertices)
                {
                    mShaderManager->Bind(ShaderType::VertexInfoShader);
                    mShaderManager->SetUniformValue("MVP", VP * model->WorldTransformation() * model->GetMeshTransformation(params.mMesh->GetName()));
                    mShaderManager->SetUniformValue("scale", params.mScale);
                    mShaderManager->SetUniformValue("nodeID", node->GetID());
                    mShaderManager->SetUniformValue("meshID", params.mMesh->GetID());
                    mShaderManager->SetUniformValue("fillVertexInfo", true);
                    params.mMesh->GetVerticesVAO()->bind();
                    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, params.mMesh->GetNumberOfVertices());
                    params.mMesh->GetVerticesVAO()->release();
                }
            }
            else
            {
                mShaderManager->Bind(ShaderType::NodeInfoShader);
                mShaderManager->SetUniformValue("MVP", VP * node->WorldTransformation() * node->GetAABB().GetTransformation());
                mShaderManager->SetUniformValue("nodeID", node->GetID());
                mShaderManager->SetUniformValue("meshID", 0);
                mShaderManager->SetUniformValue("fillVertexInfo", false);
                glBindVertexArray(mCube.mVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                mShaderManager->Release();
            }
        }
    }
}

Canavar::Engine::SelectableNodeRenderer* Canavar::Engine::SelectableNodeRenderer::Instance()
{
    static SelectableNodeRenderer instance;
    return &instance;
}