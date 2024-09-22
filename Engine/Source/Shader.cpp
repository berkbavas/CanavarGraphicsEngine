#include "Shader.h"
#include "Helper.h"

#include <QDebug>

Canavar::Engine::Shader::Shader(ShaderType type)
    : QObject()
    , mType(type)
{}

bool Canavar::Engine::Shader::Init()
{
    initializeOpenGLFunctions();

    mShaderName = mPaths.value(QOpenGLShader::Vertex).sliced(mPaths.value(QOpenGLShader::Vertex).lastIndexOf("/") + 1);

    qInfo() << mShaderName << "is initializing... ";

    mProgram = new QOpenGLShaderProgram;

    auto types = mPaths.keys();

    for (auto type : qAsConst(types))
    {
        if (!mProgram->addShaderFromSourceCode(type, Helper::GetBytes(mPaths[type])))
        {
            qWarning() << Q_FUNC_INFO << "Could not load" << GetShaderTypeString(type);
            mProgram->deleteLater();
            return false;
        }
    }

    if (mType == ShaderType::LightningStrikeShader)
    {
        const GLchar* Varyings[2];
        Varyings[0] = "outWorldPosition";
        Varyings[1] = "outForkLevel";

        mProgram->create();
        qDebug() << Q_FUNC_INFO << "Calling glTransformFeedbackVaryings()..." << "Program ID is" << mProgram->programId();
        qDebug() << mProgram->log();
        glTransformFeedbackVaryings(mProgram->programId(), 2, Varyings, GL_INTERLEAVED_ATTRIBS);

    }

    if (!mProgram->link())
    {
        qWarning() << Q_FUNC_INFO << "Could not link shader program.";
        mProgram->deleteLater();
        return false;
    }

    if (!mProgram->bind())
    {
        qWarning() << Q_FUNC_INFO << "Could not bind shader program.";
        mProgram->deleteLater();
        return false;
    }

    mProgram->release();

    qInfo() << mShaderName << "is initialized.";

    return true;
}

bool Canavar::Engine::Shader::Bind()
{
    return mProgram->bind();
}

void Canavar::Engine::Shader::Release()
{
    mProgram->release();
}

void Canavar::Engine::Shader::AddPath(QOpenGLShader::ShaderTypeBit type, const QString& path)
{
    mPaths.insert(type, path);
}

void Canavar::Engine::Shader::AddTransformFeedbackVarying(const QString& varying)
{
    mTransformFeedbackVaryings << varying;
}

void Canavar::Engine::Shader::SetUniformValue(const QString& name, int value)
{
    mProgram->setUniformValue(mProgram->uniformLocation(name), value);
}

void Canavar::Engine::Shader::SetUniformValue(const QString& name, unsigned int value)
{
    mProgram->setUniformValue(mProgram->uniformLocation(name), value);
}

void Canavar::Engine::Shader::SetUniformValue(const QString& name, float value)
{
    mProgram->setUniformValue(mProgram->uniformLocation(name), value);
}

void Canavar::Engine::Shader::SetUniformValue(const QString& name, const QVector3D& value)
{
    mProgram->setUniformValue(mProgram->uniformLocation(name), value);
}

void Canavar::Engine::Shader::SetUniformValue(const QString& name, const QVector4D& value)
{
    mProgram->setUniformValue(mProgram->uniformLocation(name), value);
}

void Canavar::Engine::Shader::SetUniformValue(const QString& name, const QMatrix4x4& value)
{
    mProgram->setUniformValue(mProgram->uniformLocation(name), value);
}

void Canavar::Engine::Shader::SetUniformValue(const QString& name, const QMatrix3x3& value)
{
    mProgram->setUniformValue(mProgram->uniformLocation(name), value);
}

void Canavar::Engine::Shader::SetUniformValueArray(const QString& name, const QVector<QVector3D>& values)
{
    mProgram->setUniformValueArray(mProgram->uniformLocation(name), values.constData(), values.size());
}

void Canavar::Engine::Shader::SetSampler(const QString& name, unsigned int unit, unsigned int id, GLenum target)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, id);
    SetUniformValue(name, unit);
}

Canavar::Engine::ShaderType Canavar::Engine::Shader::GetType() const
{
    return mType;
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