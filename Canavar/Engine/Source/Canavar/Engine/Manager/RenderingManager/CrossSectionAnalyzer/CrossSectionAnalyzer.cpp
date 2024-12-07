#include "CrossSectionAnalyzer.h"

#include "Canavar/Engine/Core/ManagerProvider.h"
#include "Canavar/Engine/Manager/CameraManager.h"
#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"

void Canavar::Engine::CrossSectionAnalyzer::Initialize(ManagerProvider* pProvider)
{
    initializeOpenGLFunctions();

    mBasicShader = pProvider->GetShaderManager()->GetShader(ShaderType::Basic);
    mCrossSectionShader = pProvider->GetShaderManager()->GetShader(ShaderType::CrossSection);

    mNodeManager = pProvider->GetNodeManager();
    mCameraManager = pProvider->GetCameraManager();

    mPlane = new Plane;

    mCrossSectionAnalyzerWidget = new CrossSectionAnalyzerWidget;
    mCrossSectionAnalyzerWidget->setWindowTitle("CrossSectionAnalyzerWidget");
    mCrossSectionAnalyzerWidget->setWindowFlag(Qt::WindowStaysOnTopHint, true);

    QSurfaceFormat format = mCrossSectionAnalyzerWidget->format();
    format.setSamples(4);
    mCrossSectionAnalyzerWidget->setFormat(format);

    connect(mCrossSectionAnalyzerWidget, &CrossSectionAnalyzerWidget::Render, this, &CrossSectionAnalyzer::RenderCrossSection);
}

void Canavar::Engine::CrossSectionAnalyzer::ShowWidget()
{
    mCrossSectionAnalyzerWidget->resize(400, 400);
    mCrossSectionAnalyzerWidget->show();
}

void Canavar::Engine::CrossSectionAnalyzer::CloseWidget()
{
    mCrossSectionAnalyzerWidget->close();
}

void Canavar::Engine::CrossSectionAnalyzer::RenderPlane()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mPlane->SetPosition(mPlanePosition);
    mPlane->SetScale(mPlaneScale);

    mBasicShader->Bind();
    mBasicShader->SetUniformValue("MVP", mCameraManager->GetActiveCamera()->GetViewProjectionMatrix() * mPlane->GetTransformation());
    mBasicShader->SetUniformValue("color", mPlaneColor);
    mPlane->Render();
    mBasicShader->Release();

    glDisable(GL_BLEND);
}

void Canavar::Engine::CrossSectionAnalyzer::RenderCrossSection()
{
    mCrossSectionProjectionMatrix.setToIdentity();
    mCrossSectionProjectionMatrix.ortho(-mPlaneScale.x(),
                                        mPlaneScale.x(), //
                                        -mPlaneScale.y(),
                                        mPlaneScale.y(),
                                        1000,
                                        0);

    mCrossSectionViewMatrix.setToIdentity();
    mCrossSectionViewMatrix.rotate(mPlane->GetRotation().conjugated());
    mCrossSectionViewMatrix.translate(-mPlane->GetPosition());

    const auto w = mCrossSectionAnalyzerWidget->width() * mCrossSectionAnalyzerWidget->devicePixelRatio();
    const auto h = mCrossSectionAnalyzerWidget->height() * mCrossSectionAnalyzerWidget->devicePixelRatio();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
    glClearColor(0, 0, 0, 1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glLineWidth(2.0f);

    mCrossSectionShader->Bind();
    mCrossSectionShader->SetUniformValue("VP", mCrossSectionProjectionMatrix * mCrossSectionViewMatrix);
    mCrossSectionShader->SetUniformValue("plane.point", mPlane->GetPosition());
    mCrossSectionShader->SetUniformValue("plane.normal", mPlane->GetRotation() * QVector3D(0, 0, -1));
    mCrossSectionShader->SetUniformValue("color", mCrossSectionColor);

    const auto& nodes = mNodeManager->GetNodes();

    for (const auto& pNode : nodes)
    {
        if (const auto pObject = std::dynamic_pointer_cast<Object>(pNode))
        {
            if (pObject->GetVisible() == false)
            {
                continue;
            }

            if (ModelPtr pModel = std::dynamic_pointer_cast<Model>(pObject))
            {
                if (const auto pScene = mNodeManager->GetScene(pModel))
                {
                    pScene->Render(pModel.get(), mCrossSectionShader);
                }
            }
        }
    }

    mCrossSectionShader->Release();
}
