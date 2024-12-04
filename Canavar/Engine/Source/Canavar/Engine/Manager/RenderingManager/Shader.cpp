#include "Shader.h"

#include "Canavar/Engine/Util/Util.h"

#include <QDebug>
#include <QFile>

Canavar::Engine::Shader::Shader(ShaderType type, const QString& name)
    : mProgram(nullptr)
    , mShaderType(type)
    , mName(name)
{}

void Canavar::Engine::Shader::Initialize()
{
    LOG_INFO("Shader::Initialize: '{}' is being initializing.", mName.toStdString());

    initializeOpenGLFunctions();

    mProgram = new QOpenGLShaderProgram;

    for (const auto& [shaderType, path] : mPaths)
    {
        const auto bytes = Util::GetBytes(path);

        if (bytes.size() == 0)
        {
            CGE_EXIT_FAILURE("Shader::Initialize: '{}' could not be found: '{}'", GetShaderTypeString(shaderType).toStdString(), mName.toStdString());
        }

        if (!mProgram->addShaderFromSourceCode(shaderType, bytes))
        {
            CGE_EXIT_FAILURE("Shader::Initialize: '{}' could not be loaded: '{}'", GetShaderTypeString(shaderType).toStdString(), mName.toStdString());
        }
    }

    if (mCallback != nullptr)
    {
        LOG_DEBUG("Shader::Initialize: Calling callback...");
        mCallback(QOpenGLContext::currentContext(), mProgram);
        LOG_DEBUG("Shader::Initialize: Callback returned.");
    }

    if (!mProgram->link())
    {
        CGE_EXIT_FAILURE("Shader::Initialize: Could not link shader program: '{}'", mName.toStdString());
    }

    if (!mProgram->bind())
    {
        CGE_EXIT_FAILURE("Shader::Initialize: Could not bind shader program: '{}'", mName.toStdString());
    }

    LOG_INFO("Shader::Initialize: '{}' has been initialized.", mName.toStdString());
}

bool Canavar::Engine::Shader::Bind()
{
    return mProgram->bind();
}

void Canavar::Engine::Shader::Release()
{
    mProgram->release();
}

void Canavar::Engine::Shader::SetCallbackBeforeLinking(const Callback& callback)
{
    mCallback = callback;
}

void Canavar::Engine::Shader::AddPath(QOpenGLShader::ShaderTypeBit type, const QString& path)
{
    mPaths.emplace(type, path);
}

QString Canavar::Engine::Shader::GetName() const
{
    return mName;
}

Canavar::Engine::ShaderType Canavar::Engine::Shader::GetShaderType() const
{
    return mShaderType;
}

QString Canavar::Engine::Shader::GetShaderTypeString(QOpenGLShader::ShaderTypeBit type)
{
    switch (type)
    {
    case QOpenGLShader::Vertex:
        return "Vertex Shader";
    case QOpenGLShader::Fragment:
        return "Fragment Shader";
    case QOpenGLShader::Geometry:
        return "Geometry Shader";
    case QOpenGLShader::TessellationControl:
        return "Tessellation Control Shader";
    case QOpenGLShader::TessellationEvaluation:
        return "Tesselation Evaluation Shader";
    case QOpenGLShader::Compute:
        return "Compute Shader";
    default:
        return "Unknown Shader";
    }
}

void Canavar::Engine::Shader::SetSampler(const QString& name, GLuint unit, GLuint textureId, GLuint target)
{
    SetUniformValue(name, unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, textureId);
}
