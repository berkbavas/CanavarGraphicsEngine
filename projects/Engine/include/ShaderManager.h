#pragma once

#include "Common.h"
#include "Manager.h"

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

namespace Canavar {
    namespace Engine {
        class Shader;

        class ShaderManager : public Manager
        {
        private:
            ShaderManager();

        public:
            static ShaderManager* Instance();

            Shader* GetShader(ShaderType shader);

            bool Init() override;

            bool Bind(ShaderType shader);
            void Release();

            void SetUniformValue(const QString& name, int value);
            void SetUniformValue(const QString& name, unsigned int value);
            void SetUniformValue(const QString& name, float value);
            void SetUniformValue(const QString& name, const QVector3D& value);
            void SetUniformValue(const QString& name, const QVector4D& value);
            void SetUniformValue(const QString& name, const QMatrix4x4& value);
            void SetUniformValue(const QString& name, const QMatrix3x3& value);
            void SetUniformValueArray(const QString& name, const QVector<QVector3D>& values);
            void SetSampler(const QString& name, unsigned int unit, unsigned int id, GLenum target = GL_TEXTURE_2D);

        private:
            ShaderType mActiveShader;
            QMap<ShaderType, Shader*> mShaders;
        };
    } // namespace Engine
} // namespace Canavar
