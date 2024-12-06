#pragma once

#include "Canavar/Engine/Util/Logger.h"

#include <functional>
#include <map>

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>

namespace Canavar::Engine
{
    enum class ShaderType
    {
        Model,
        Sky,
        Terrain,
        Blur,
        PostProcess,
        NozzleEffect,
        Line,
        LightningStrike,
        ShadowMapping,
        Bitmap,
        PointCloud,
        VertexPainter,
        Basic,
        CrossSection
    };

    class Shader : public QOpenGLExtraFunctions
    {
        using Callback = std::function<void(QOpenGLContext*, QOpenGLShaderProgram*)>;

      public:
        Shader(ShaderType type, const QString& name);

        void Initialize();
        bool Bind();
        void Release();
        void SetCallbackBeforeLinking(const Callback& callback);

        void AddPath(QOpenGLShader::ShaderTypeBit type, const QString& path);

        QString GetName() const;
        ShaderType GetShaderType() const;

        static QString GetShaderTypeString(QOpenGLShader::ShaderTypeBit type);

        template<typename T>
        void SetUniformValue(const QString& name, T value)
        {
            const auto location = mProgram->uniformLocation(name);

            if (0 <= location)
            {
                mProgram->setUniformValue(location, value);
            }
            else
            {
                LOG_FATAL("Shader::SetUniformValue: [{}] Uniform location '{}' could not be found.", mName.toStdString(), name.toStdString());
            }
        }

        template<typename T>
        void SetUniformValueArray(const QString& name, const QVector<T>& values)
        {
            const auto location = mProgram->uniformLocation(name);

            if (0 <= location)
            {
                mProgram->setUniformValueArray(location, values.constData(), values.size());
            }
            else
            {
                LOG_FATAL("Shader::SetUniformValue: Uniform location '{}' could not be found.", name.toStdString());
            }
        }

        void SetSampler(const QString& name, GLuint unit, GLuint textureId, GLuint target = GL_TEXTURE_2D);

      private:
        Callback mCallback{ nullptr };
        QOpenGLShaderProgram* mProgram;
        std::map<QOpenGLShader::ShaderTypeBit, QString> mPaths;

        const ShaderType mShaderType;

        const QString mName;
    };
}
