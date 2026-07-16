#pragma once

#include "Canavar/Engine/Util/Logger.h"

#include <functional>
#include <map>
#include <memory>

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>

namespace Canavar::Engine
{
    class Shader : public QOpenGLExtraFunctions
    {
        using Callback = std::function<void(QOpenGLContext*, QOpenGLShaderProgram*)>;
        static constexpr int MINIMUM_VALID_LOCATION = 0; // Minimum valid uniform location (0 or higher). Negative values indicate an invalid location.

      public:
        explicit Shader(const QString& Name);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        void Initialize();
        bool Bind();
        void Unbind();
        void SetCallbackBeforeLinking(const Callback& Callback);
        void AddPath(QOpenGLShader::ShaderTypeBit Type, const QString& Path);
        QString GetName() const;
        static QString GetShaderTypeString(QOpenGLShader::ShaderTypeBit Type);

        template<typename T>
        void SetUniform(const QString& Name, const T& Value)
        {
            const auto Location = mProgram->uniformLocation(Name);

            if (MINIMUM_VALID_LOCATION <= Location)
            {
                mProgram->setUniformValue(Location, Value);
            }
            else
            {
                LOG_FATAL("Shader::SetUniform: [{}] Uniform location '{}' could not be found.", mName.toStdString(), Name.toStdString());
            }
        }

        template<typename T>
        void SetUniformArray(const QString& Name, const QVector<T>& Values)
        {
            const auto Location = mProgram->uniformLocation(Name);

            if (MINIMUM_VALID_LOCATION <= Location)
            {
                mProgram->setUniformValueArray(Location, Values.constData(), Values.size());
            }
            else
            {
                LOG_FATAL("Shader::SetUniformArray: [{}] Uniform location '{}' could not be found.", mName.toStdString(), Name.toStdString());
            }
        }

        void SetUniformArray(const QString& Name, const GLfloat* Values, int Count, int TupleSize)
        {
            const auto Location = mProgram->uniformLocation(Name);

            if (0 <= Location)
            {
                mProgram->setUniformValueArray(Location, Values, Count, TupleSize);
            }
            else
            {
                LOG_FATAL("Shader::SetUniformValueArray[{}]: Uniform location '{}' could not be found.", mName.toStdString(), Name.toStdString());
            }
        }

        void SetSampler(const QString& Name, GLuint Unit, GLuint TextureId, GLuint Target = GL_TEXTURE_2D);

      private:
        QString mName;
        std::unique_ptr<QOpenGLShaderProgram> mProgram;
        std::map<QOpenGLShader::ShaderTypeBit, QString> mPaths;
        Callback mCallback{ nullptr };
    };

    using ShaderPtr = std::unique_ptr<Shader>;
}
