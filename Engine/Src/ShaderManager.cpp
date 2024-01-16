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

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/ModelColored.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/ModelColored.frag");

        if (!shader->Init())
            return false;
    }

    // Model Textured Shader
    {
        Shader* shader = new Shader(ShaderType::ModelTexturedShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/ModelTextured.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/ModelTextured.frag");

        if (!shader->Init())
            return false;
    }

    // Sky Shader
    {
        Shader* shader = new Shader(ShaderType::SkyShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Sky.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Sky.frag");

        if (!shader->Init())
            return false;
    }

    // Terrain Shader
    {
        Shader* shader = new Shader(ShaderType::TerrainShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");
        shader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tcs");
        shader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tes");

        if (!shader->Init())
            return false;
    }

    // Blur Shader
    {
        Shader* shader = new Shader(ShaderType::BlurShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Blur.frag");

        if (!shader->Init())
            return false;
    }

    // Post Process Shader
    {
        Shader* shader = new Shader(ShaderType::PostProcessShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/PostProcess.frag");

        if (!shader->Init())
            return false;
    }

    // Screen Shader
    {
        Shader* shader = new Shader(ShaderType::ScreenShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Screen.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Screen.frag");

        if (!shader->Init())
            return false;
    }

    // Node Info Shader
    {
        Shader* shader = new Shader(ShaderType::NodeInfoShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NodeInfo.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NodeInfo.frag");

        if (!shader->Init())
            return false;
    }

    // Nozzle Effect Shader
    {
        Shader* shader = new Shader(ShaderType::NozzleEffectShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NozzleEffect.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NozzleEffect.frag");

        if (!shader->Init())
            return false;
    }

    // Firecracker Effect Shader
    {
        Shader* shader = new Shader(ShaderType::FirecrackerEffectShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/FirecrackerEffect.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/FirecrackerEffect.frag");

        if (!shader->Init())
            return false;
    }

    // Basic Shader
    {
        Shader* shader = new Shader(ShaderType::BasicShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Basic.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Basic.frag");

        if (!shader->Init())
            return false;
    }

    // Mesh Vertex Renderer Shader
    {
        Shader* shader = new Shader(ShaderType::MeshVertexRendererShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/MeshVertexRenderer.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/MeshVertexRenderer.frag");

        if (!shader->Init())
            return false;
    }

    // Node Info Shader
    {
        Shader* shader = new Shader(ShaderType::VertexInfoShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/MeshVertexRenderer.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NodeInfo.frag");

        if (!shader->Init())
            return false;
    }

    // Line Strip Shader
    {
        Shader* shader = new Shader(ShaderType::LineStripShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/LineStrip.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/LineStrip.frag");

        if (!shader->Init())
            return false;
    }

    // Raycaster Shader
    {
        Shader* shader = new Shader(ShaderType::RaycasterShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Raycaster.vert");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Raycaster.frag");

        if (!shader->Init())
            return false;
    }

    // Lightning Strike Shader
    {
        Shader* shader = new Shader(ShaderType::LightningStrikeShader);
        mShaders.insert(shader->GetType(), shader);

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/LightningStrike.vert");
        shader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/LightningStrike.geom");

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

        shader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/LightningStrikeQuadGenerator.vert");
        shader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/LightningStrikeQuadGenerator.geom");
        shader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/LightningStrikeQuadGenerator.frag");

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