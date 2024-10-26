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

    mShaders.emplace(std::pair(ShaderType::Model, mModelShader));
    mShaders.emplace(std::pair(ShaderType::Sky, mSkyShader));
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