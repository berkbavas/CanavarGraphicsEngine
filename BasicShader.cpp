#include "BasicShader.h"
#include "Helper.h"

BasicShader::BasicShader(QObject *parent)
    : QObject{parent}
    , mProgram(nullptr)
{}

BasicShader::~BasicShader()
{
    if (mProgram)
        mProgram->deleteLater();

    mProgram = nullptr;
}

bool BasicShader::init()
{
    initializeOpenGLFunctions();

    mProgram = new QOpenGLShaderProgram;

    if (!mProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, Helper::getBytes(":/Resources/Shaders/Basic/VertexShader.vert")))
    {
        qWarning() << Q_FUNC_INFO << "Could not load vertex shader.";
        return false;
    }

    if (!mProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, Helper::getBytes(":/Resources/Shaders/Basic/FragmentShader.frag")))
    {
        qWarning() << Q_FUNC_INFO << "Could not load fragment shader.";
        return false;
    }

    if (!mProgram->link())
    {
        qWarning() << Q_FUNC_INFO << "Could not link shader program.";
        return false;
    }

    if (!mProgram->bind())
    {
        qWarning() << Q_FUNC_INFO << "Could not bind shader program.";
        return false;
    }

    mLocations.insert("light.color", mProgram->uniformLocation("light.color"));
    mLocations.insert("light.position", mProgram->uniformLocation("light.position"));
    mLocations.insert("light.ambient", mProgram->uniformLocation("light.ambient"));
    mLocations.insert("light.diffuse", mProgram->uniformLocation("light.diffuse"));
    mLocations.insert("light.specular", mProgram->uniformLocation("light.specular"));

    mLocations.insert("node.transformation", mProgram->uniformLocation("node.transformation"));
    mLocations.insert("node.color", mProgram->uniformLocation("node.color"));
    mLocations.insert("node.ambient", mProgram->uniformLocation("node.ambient"));
    mLocations.insert("node.diffuse", mProgram->uniformLocation("node.diffuse"));
    mLocations.insert("node.specular", mProgram->uniformLocation("node.specular"));
    mLocations.insert("node.shininess", mProgram->uniformLocation("node.shininess"));

    mLocations.insert("cameraPosition", mProgram->uniformLocation("cameraPosition"));
    mLocations.insert("viewMatrix", mProgram->uniformLocation("viewMatrix"));

    mLocations.insert("projectionMatrix", mProgram->uniformLocation("projectionMatrix"));

    mProgram->bindAttributeLocation("vPosition", 0);
    mProgram->bindAttributeLocation("vNormal", 1);

    mProgram->release();

    qInfo() << Q_FUNC_INFO << "BasicShader is initialized.";
    qInfo() << Q_FUNC_INFO << "Uniform locations are:" << mLocations;

    return true;
}

bool BasicShader::bind()
{
    return mProgram->bind();
}

void BasicShader::release()
{
    mProgram->release();
}

void BasicShader::setUniformValue(const QString &name, float value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void BasicShader::setUniformValue(const QString &name, const QVector3D &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void BasicShader::setUniformValue(const QString &name, const QVector4D &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}

void BasicShader::setUniformValue(const QString &name, const QMatrix4x4 &value)
{
    mProgram->setUniformValue(mLocations[name], value);
}
