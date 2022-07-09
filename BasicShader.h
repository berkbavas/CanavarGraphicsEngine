#ifndef BASICSHADER_H
#define BASICSHADER_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class BasicShader : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit BasicShader(QObject *parent = nullptr);
    ~BasicShader();

    bool init();
    bool bind();
    void release();

    void setUniformValue(const QString &name, float value);
    void setUniformValue(const QString &name, const QVector3D &value);
    void setUniformValue(const QString &name, const QVector4D &value);
    void setUniformValue(const QString &name, const QMatrix4x4 &value);

private:
    QOpenGLShaderProgram *mProgram;
    QMap<QString, GLuint> mLocations;
};

#endif // BASICSHADER_H
