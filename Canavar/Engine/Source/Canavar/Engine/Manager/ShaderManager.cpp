#include "ShaderManager.h"

Canavar::Engine::ShaderManager::ShaderManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::ShaderManager::Initialize()
{
    mModelShader = new Shader("Model Shader");
    mModelShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Model.vert");
    mModelShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Model.frag");
    mModelShader->Initialize();

    mSkyShader = new Shader("Sky Shader");
    mSkyShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Sky.vert");
    mSkyShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Sky.frag");
    mSkyShader->Initialize();

    mTerrainShader = new Shader("Terrain Shader");
    mTerrainShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mTerrainShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tcs");
    mTerrainShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tes");
    mTerrainShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");
    mTerrainShader->Initialize();

    mBlurShader = new Shader("Blur Shader");
    mBlurShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mBlurShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Blur.frag");
    mBlurShader->Initialize();

    mPostProcessShader = new Shader("Post Process Shader");
    mPostProcessShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mPostProcessShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/PostProcess.frag");
    mPostProcessShader->Initialize();

    mNozzleEffect = new Shader("Nozzle Effect Shader");
    mNozzleEffect->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NozzleEffect.vert");
    mNozzleEffect->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NozzleEffect.frag");
    mNozzleEffect->Initialize();

    mLineShader = new Shader("Line Shader");
    mLineShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Line.vert");
    mLineShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Line.frag");
    mLineShader->Initialize();

    mLightningStrikeShader = new Shader("Lightning Strike Shader");
    mLightningStrikeShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/LightningStrike.vert");
    mLightningStrikeShader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/LightningStrike.geom");

    mLightningStrikeShader->SetCallbackBeforeLinking([=](QOpenGLShaderProgram* pProgram) {
        const GLchar* Varyings[2];
        Varyings[0] = "outWorldPosition";
        Varyings[1] = "outForkLevel";

        pProgram->create();
        qDebug() << "Calling glTransformFeedbackVaryings()  with Program ID" << pProgram->programId();
        qDebug() << "QOpenGLShaderProgram log is: " << pProgram->log();
        mLightningStrikeShader->glTransformFeedbackVaryings(pProgram->programId(), 2, Varyings, GL_INTERLEAVED_ATTRIBS);
    });

    mLightningStrikeShader->Initialize();

    mLightningStrikeQuadGeneratorShader = new Shader("Lightning Strike Quad Generator Shader");
    mLightningStrikeQuadGeneratorShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/LightningStrikeQuadGenerator.vert");
    mLightningStrikeQuadGeneratorShader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/LightningStrikeQuadGenerator.geom");
    mLightningStrikeQuadGeneratorShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/LightningStrikeQuadGenerator.frag");
    mLightningStrikeQuadGeneratorShader->Initialize();

    mShaders.emplace(std::pair(ShaderType::Model, mModelShader));
    mShaders.emplace(std::pair(ShaderType::Sky, mSkyShader));
    mShaders.emplace(std::pair(ShaderType::Terrain, mTerrainShader));
    mShaders.emplace(std::pair(ShaderType::Blur, mBlurShader));
    mShaders.emplace(std::pair(ShaderType::PostProcess, mPostProcessShader));
    mShaders.emplace(std::pair(ShaderType::NozzleEffect, mNozzleEffect));
    mShaders.emplace(std::pair(ShaderType::Line, mLineShader));
    mShaders.emplace(std::pair(ShaderType::LightningStrike, mLightningStrikeShader));
    mShaders.emplace(std::pair(ShaderType::LightningStrikeQuadGenerator, mLightningStrikeQuadGeneratorShader));
}

Canavar::Engine::Shader* Canavar::Engine::ShaderManager::GetShader(ShaderType type)
{
    Shader* pResult = nullptr;

    if (const auto it = mShaders.find(type); it != mShaders.end())
    {
        pResult = it->second;
    }

    return pResult;
}