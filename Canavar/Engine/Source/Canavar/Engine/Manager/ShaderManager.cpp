#include "ShaderManager.h"

Canavar::Engine::ShaderManager::ShaderManager(QObject* parent)
    : Manager(parent)
{}

void Canavar::Engine::ShaderManager::Initialize()
{
    mModelShader = new Shader(ShaderType::Model, "Model Shader");
    mModelShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Model.vert");
    mModelShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Model.frag");
    mModelShader->Initialize();

    mSkyShader = new Shader(ShaderType::Sky, "Sky Shader");
    mSkyShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Sky.vert");
    mSkyShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Sky.frag");
    mSkyShader->Initialize();

    mTerrainShader = new Shader(ShaderType::Terrain, "Terrain Shader");
    mTerrainShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mTerrainShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tcs");
    mTerrainShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tes");
    mTerrainShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");
    mTerrainShader->Initialize();

    mBlurShader = new Shader(ShaderType::Blur, "Blur Shader");
    mBlurShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mBlurShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Blur.frag");
    mBlurShader->Initialize();

    mPostProcessShader = new Shader(ShaderType::PostProcess, "Post Process Shader");
    mPostProcessShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mPostProcessShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/PostProcess.frag");
    mPostProcessShader->Initialize();

    mNozzleEffect = new Shader(ShaderType::NozzleEffect, "Nozzle Effect Shader");
    mNozzleEffect->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/NozzleEffect.vert");
    mNozzleEffect->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/NozzleEffect.frag");
    mNozzleEffect->Initialize();

    mLineShader = new Shader(ShaderType::Line, "Line Shader");
    mLineShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Line.vert");
    mLineShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Line.frag");
    mLineShader->Initialize();

    mLightningStrikeShader = new Shader(ShaderType::LightningStrike, "Lightning Strike Shader");
    mLightningStrikeShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/LightningStrike.vert");
    mLightningStrikeShader->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/LightningStrike.geom");

    mLightningStrikeShader->SetCallbackBeforeLinking([=](QOpenGLContext* pContext, QOpenGLShaderProgram* pProgram) {
        const GLchar* Varyings[2];
        Varyings[0] = "outWorldPosition";
        Varyings[1] = "outForkLevel";

        pProgram->create();
        qDebug() << "Calling glTransformFeedbackVaryings()  with Program ID" << pProgram->programId();
        qDebug() << "QOpenGLShaderProgram log is: " << pProgram->log();
        pContext->extraFunctions()->glTransformFeedbackVaryings(pProgram->programId(), 2, Varyings, GL_INTERLEAVED_ATTRIBS);
    });

    mLightningStrikeShader->Initialize();

    mShadowMappingShader = new Shader(ShaderType::ShadowMapping, "Shadow Mapping Shader");
    mShadowMappingShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/ShadowMapping.vert");
    mShadowMappingShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/ShadowMapping.frag");
    mShadowMappingShader->Initialize();

    mBasicShader = new Shader(ShaderType::Basic, "Basic Shader");
    mBasicShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Basic.vert");
    mBasicShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Basic.frag");
    mBasicShader->Initialize();

    mCrossSection = new Shader(ShaderType::CrossSection, "Cross Section Shader");
    mCrossSection->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/CrossSection.vert");
    mCrossSection->AddPath(QOpenGLShader::Geometry, ":/Resources/Shaders/CrossSection.geom");
    mCrossSection->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/CrossSection.frag");
    mCrossSection->Initialize();

    // Emplace
    mShaders.emplace(std::pair(ShaderType::Model, mModelShader));
    mShaders.emplace(std::pair(ShaderType::Sky, mSkyShader));
    mShaders.emplace(std::pair(ShaderType::Terrain, mTerrainShader));
    mShaders.emplace(std::pair(ShaderType::Blur, mBlurShader));
    mShaders.emplace(std::pair(ShaderType::PostProcess, mPostProcessShader));
    mShaders.emplace(std::pair(ShaderType::NozzleEffect, mNozzleEffect));
    mShaders.emplace(std::pair(ShaderType::Line, mLineShader));
    mShaders.emplace(std::pair(ShaderType::LightningStrike, mLightningStrikeShader));
    mShaders.emplace(std::pair(ShaderType::ShadowMapping, mShadowMappingShader));
    mShaders.emplace(std::pair(ShaderType::Basic, mBasicShader));
    mShaders.emplace(std::pair(ShaderType::CrossSection, mCrossSection));
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