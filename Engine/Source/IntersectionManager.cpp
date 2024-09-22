#include "IntersectionManager.h"
#include "NodeManager.h"
#include "ShaderManager.h"

Canavar::Engine::IntersectionManager::IntersectionManager()
    : Manager()
{}

Canavar::Engine::IntersectionManager *Canavar::Engine::IntersectionManager::Instance()
{
    static IntersectionManager instance;

    return &instance;
}

bool Canavar::Engine::IntersectionManager::Init()
{
    initializeOpenGLFunctions();

    mNodeManager = NodeManager::Instance();
    mShaderManager = ShaderManager::Instance();

    mFBOFormat.setSamples(0);
    mFBOFormat.setInternalTextureFormat(GL_RGBA32F);
    mFBOFormat.setAttachment(QOpenGLFramebufferObject::Depth);

    mFBO = new QOpenGLFramebufferObject(FBO_WIDTH, FBO_HEIGHT, mFBOFormat);

    mProjection.ortho(-10, 10, -10, 10, 0.1, 1000000);

    return true;
}

Canavar::Engine::IntersectionManager::IntersectionResult Canavar::Engine::IntersectionManager::Raycast(const QVector3D &rayOrigin,
                                                                                                       const QVector3D &rayDirection,
                                                                                                       const QList<Model *> &includeList,
                                                                                                       const QList<Model *> &excludeList)
{
    auto dir = rayDirection.normalized();
    float x = dir.x();
    float y = dir.y();
    float z = dir.z();
    float theta = qRadiansToDegrees(atan2(x, -z));
    float phi = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

    QMatrix4x4 view;
    view.rotate(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), -theta) * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), phi));
    view.setColumn(3, QVector4D(rayOrigin, 1));

    auto vp = mProjection * view.inverted();

    mFBO->bind();
    glViewport(0, 0, FBO_WIDTH, FBO_HEIGHT);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mShaderManager->Bind(ShaderType::RaycasterShader);
    mShaderManager->SetUniformValue("VP", vp);

    if (includeList.isEmpty())
    {
        auto &nodes = mNodeManager->GetNodes();

        for (const auto &node : nodes)
        {
            if (!node->GetVisible())
                continue;

            if (auto model = dynamic_cast<Model *>(node))
            {
                if (excludeList.contains(model))
                    continue;

                model->Render(RenderMode::Raycaster);
            }
        }

    } else
    {
        for (const auto &node : includeList)
            if (auto model = dynamic_cast<Model *>(node))
                model->Render(RenderMode::Raycaster);
    }

    IntersectionResult result;
    QVector4D point;

    glReadPixels(0.5 * FBO_WIDTH, 0.5 * FBO_HEIGHT, 1, 1, GL_RGBA, GL_FLOAT, &point);

    mShaderManager->Release();
    mFBO->release();

    if (qFuzzyCompare(point.w(), 1.0f))
    {
        result.success = true;
        result.point = point.toVector3D();
    } else
    {
        result.success = false;
    }

    return result;
}

const int Canavar::Engine::IntersectionManager::FBO_WIDTH = 1024;
const int Canavar::Engine::IntersectionManager::FBO_HEIGHT = 1024;
