#include "ShaderManager.h"
#include "Shader.h"

Canavar::Engine::ShaderManager::ShaderManager()
    : Manager()
    , mActiveShader(ShaderType::None)
{}

Canavar::Engine::Shader* Canavar::Engine::ShaderManager::GetShader(ShaderType shader)
{
    return mShaders.value(shader);
}

bool Canavar::Engine::ShaderManager::Init()
{
    // Model Colored Shader
    {
        Shader* shader = new Shader(ShaderType::ModelColoredShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/ModelColored.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/ModelColored.frag");

        if (!shader->Init())
            return false;
    }

    // Model Textured Shader
    {
        Shader* shader = new Shader(ShaderType::ModelTexturedShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/ModelTextured.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/ModelTextured.frag");

        if (!shader->Init())
            return false;
    }

    // Sky Shader
    {
        Shader* shader = new Shader(ShaderType::SkyShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/Sky.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/Sky.frag");

        if (!shader->Init())
            return false;
    }

    // Terrain Shader
    {
        Shader* shader = new Shader(ShaderType::TerrainShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/Terrain.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/Terrain.frag");
        shader->AddPath(QOpenGLShader::TessellationControl, ":/resources/shaders/Terrain.tcs");
        shader->AddPath(QOpenGLShader::TessellationEvaluation, ":/resources/shaders/Terrain.tes");

        if (!shader->Init())
            return false;
    }

    // Blur Shader
    {
        Shader* shader = new Shader(ShaderType::BlurShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/Quad.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/Blur.frag");

        if (!shader->Init())
            return false;
    }

    // Post Process Shader
    {
        Shader* shader = new Shader(ShaderType::PostProcessShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/Quad.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/PostProcess.frag");

        if (!shader->Init())
            return false;
    }

    // Screen Shader
    {
        Shader* shader = new Shader(ShaderType::ScreenShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/Screen.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/Screen.frag");

        if (!shader->Init())
            return false;
    }

    // Node Info Shader
    {
        Shader* shader = new Shader(ShaderType::NodeInfoShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/NodeInfo.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/NodeInfo.frag");

        if (!shader->Init())
            return false;
    }

    // Nozzle Effect Shader
    {
        Shader* shader = new Shader(ShaderType::NozzleEffectShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/NozzleEffect.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/NozzleEffect.frag");

        if (!shader->Init())
            return false;
    }

    // Firecracker Effect Shader
    {
        Shader* shader = new Shader(ShaderType::FirecrackerEffectShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/FirecrackerEffect.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/FirecrackerEffect.frag");

        if (!shader->Init())
            return false;
    }

    // Basic Shader
    {
        Shader* shader = new Shader(ShaderType::BasicShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/Basic.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/Basic.frag");

        if (!shader->Init())
            return false;
    }

    // Mesh Vertex Renderer Shader
    {
        Shader* shader = new Shader(ShaderType::MeshVertexRendererShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/MeshVertexRenderer.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/MeshVertexRenderer.frag");

        if (!shader->Init())
            return false;
    }

    // Node Info Shader
    {
        Shader* shader = new Shader(ShaderType::VertexInfoShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/MeshVertexRenderer.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/NodeInfo.frag");

        if (!shader->Init())
            return false;
    }

    // Line Strip Shader
    {
        Shader* shader = new Shader(ShaderType::LineStripShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/LineStrip.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/LineStrip.frag");

        if (!shader->Init())
            return false;
    }

    // Raycaster Shader
    {
        Shader* shader = new Shader(ShaderType::RaycasterShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/Raycaster.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/Raycaster.frag");

        if (!shader->Init())
            return false;
    }

    // Lightning Strike Shader
    {
        Shader* shader = new Shader(ShaderType::LightningStrikeShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/LightningStrike.vert");
        shader->AddPath(QOpenGLShader::Geometry, ":/resources/shaders/LightningStrike.geom");

        shader->AddTransformFeedbackVarying("outWorldPosition");
        shader->AddTransformFeedbackVarying("outForkLevel");

        if (false == shader->Init())
        {
            return false;
        }

    }

    // Lightning Strike Quad Generator Shader
    {
        Shader* shader = new Shader(ShaderType::LightningStrikeQuadGeneratorShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/resources/shaders/LightningStrikeQuadGenerator.vert");
        shader->AddPath(QOpenGLShader::Geometry, ":/resources/shaders/LightningStrikeQuadGenerator.geom");
        shader->AddPath(QOpenGLShader::Fragment, ":/resources/shaders/LightningStrikeQuadGenerator.frag");

        if (false == shader->Init())
        {
            return false;
        }
    }

    return true;
}

bool Canavar::Engine::ShaderManager::Bind(ShaderType shader)
{
    mActiveShader = shader;
    return mShaders.value(mActiveShader)->Bind();
}

void Canavar::Engine::ShaderManager::Release()
{
    mShaders.value(mActiveShader)->Release();
}

void Canavar::Engine::ShaderManager::SetUniformValue(const QString& name, int value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::SetUniformValue(const QString& name, unsigned int value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::SetUniformValue(const QString& name, float value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::SetUniformValue(const QString& name, const QVector3D& value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::SetUniformValue(const QString& name, const QVector4D& value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::SetUniformValue(const QString& name, const QMatrix4x4& value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::SetUniformValue(const QString& name, const QMatrix3x3& value)
{
    mShaders.value(mActiveShader)->SetUniformValue(name, value);
}

void Canavar::Engine::ShaderManager::SetUniformValueArray(const QString& name, const QVector<QVector3D>& values)
{
    mShaders.value(mActiveShader)->SetUniformValueArray(name, values);
}

void Canavar::Engine::ShaderManager::SetSampler(const QString& name, unsigned int unit, unsigned int id, GLenum target)
{
    mShaders.value(mActiveShader)->SetSampler(name, unit, id, target);
}

Canavar::Engine::ShaderManager* Canavar::Engine::ShaderManager::Instance()
{
    static ShaderManager instance;

    return &instance;
}