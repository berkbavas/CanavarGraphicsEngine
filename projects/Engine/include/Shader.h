#pragma once

#include "Common.h"

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>

namespace Canavar {
    namespace Engine {
        class Shader : public QObject, protected QOpenGLExtraFunctions
        {
        public:
            Shader(ShaderType type);

            bool Init();
            bool Bind();
            void Release();

            void AddPath(QOpenGLShader::ShaderTypeBit type, const QString& path);
            void AddTransformFeedbackVarying(const QString& varying);

            void SetUniformValue(const QString& name, int value);
            void SetUniformValue(const QString& name, unsigned int value);
            void SetUniformValue(const QString& name, float value);
            void SetUniformValue(const QString& name, const QVector3D& value);
            void SetUniformValue(const QString& name, const QVector4D& value);
            void SetUniformValue(const QString& name, const QMatrix4x4& value);
            void SetUniformValue(const QString& name, const QMatrix3x3& value);
            void SetUniformValueArray(const QString& name, const QVector<QVector3D>& values);
            void SetSampler(const QString& name, unsigned int unit, unsigned int id, GLenum target = GL_TEXTURE_2D);

            ShaderType GetType() const;

        private:
            QString GetShaderTypeString(QOpenGLShader::ShaderTypeBit type);

        private:
            QOpenGLShaderProgram* mProgram;
            QMap<QOpenGLShader::ShaderTypeBit, QString> mPaths;

            ShaderType mType;
            QString mShaderName;

            QStringList mTransformFeedbackVaryings;
        };
    } // namespace Engine
} // namespace Canavar
