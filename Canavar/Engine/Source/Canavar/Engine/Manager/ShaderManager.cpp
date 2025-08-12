#include "ShaderManager.h"

Canavar::Engine::ShaderManager::ShaderManager(QObject* pParent)
    : Manager(pParent)
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

    mLightningStrikeShader->SetCallbackBeforeLinking([=](QOpenGLContext* pContext, QOpenGLShaderProgram* pProgram) //
                                                     {
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

    mTerrainShader = new Shader(ShaderType::Terrain, "Terrain Shader");
    mTerrainShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mTerrainShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tesc");
    mTerrainShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tese");
    mTerrainShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");
    mTerrainShader->Initialize();

    mText2DShader = new Shader(ShaderType::Text2D, "2D Text Shader");
    mText2DShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Text2D.vert");
    mText2DShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Text2D.frag");
    mText2DShader->Initialize();

    mText3DShader = new Shader(ShaderType::Text2D, "3D Text Shader");
    mText3DShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Text3D.vert");
    mText3DShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Text3D.frag");
    mText3DShader->Initialize();

    mCinematicShader = new Shader(ShaderType::Cinematic, "Cinematic Shader");
    mCinematicShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mCinematicShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Cinematic.frag");
    mCinematicShader->Initialize();

    mBrightPassShader = new Shader(ShaderType::BrightPass, "Bright Pass Shader");
    mBrightPassShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mBrightPassShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/BrightPass.frag");
    mBrightPassShader->Initialize();

    mGodRaysShader = new Shader(ShaderType::GodRays, "God Rays Shader");
    mGodRaysShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mGodRaysShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/GodRays.frag");
    mGodRaysShader->Initialize();

    mCompositionShader = new Shader(ShaderType::Composition, "Composition Shader");
    mCompositionShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mCompositionShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Composition.frag");
    mCompositionShader->Initialize();

    mAcesShader = new Shader(ShaderType::Aces, "ACES Shader");
    mAcesShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mAcesShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Aces.frag");
    mAcesShader->Initialize();

    mMotionBlurShader = new Shader(ShaderType::MotionBlur, "Motion Blur Shader");
    mMotionBlurShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Quad.vert");
    mMotionBlurShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/MotionBlur.frag");
    mMotionBlurShader->Initialize();

    // Emplace
    mShaders.emplace(std::pair(ShaderType::Model, mModelShader));
    mShaders.emplace(std::pair(ShaderType::Sky, mSkyShader));
    mShaders.emplace(std::pair(ShaderType::NozzleEffect, mNozzleEffect));
    mShaders.emplace(std::pair(ShaderType::Line, mLineShader));
    mShaders.emplace(std::pair(ShaderType::LightningStrike, mLightningStrikeShader));
    mShaders.emplace(std::pair(ShaderType::ShadowMapping, mShadowMappingShader));
    mShaders.emplace(std::pair(ShaderType::Basic, mBasicShader));
    mShaders.emplace(std::pair(ShaderType::CrossSection, mCrossSection));
    mShaders.emplace(std::pair(ShaderType::Terrain, mTerrainShader));
    mShaders.emplace(std::pair(ShaderType::Text2D, mText2DShader));
    mShaders.emplace(std::pair(ShaderType::Text3D, mText3DShader));
    mShaders.emplace(std::pair(ShaderType::Cinematic, mCinematicShader));
    mShaders.emplace(std::pair(ShaderType::BrightPass, mBrightPassShader));
    mShaders.emplace(std::pair(ShaderType::GodRays, mGodRaysShader));
    mShaders.emplace(std::pair(ShaderType::Composition, mCompositionShader));
    mShaders.emplace(std::pair(ShaderType::Aces, mAcesShader));
    mShaders.emplace(std::pair(ShaderType::MotionBlur, mMotionBlurShader));
}

void Canavar::Engine::ShaderManager::Shutdown()
{
    for (const auto& [type, pShader] : mShaders)
    {
        delete pShader;
    }

    mShaders.clear();
}

Canavar::Engine::Shader* Canavar::Engine::ShaderManager::GetShader(ShaderType shaderType)
{
    for (const auto [type, pShader] : mShaders)
    {
        if (type == shaderType)
        {
            return pShader;
        }
    }

    return nullptr;
}