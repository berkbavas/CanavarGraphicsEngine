#pragma once

#include "Canavar/Engine/Core/Enums.h"
#include "Canavar/Engine/Util/Logger.h"

#include <functional>
#include <map>

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>

namespace Canavar::Engine
{

    class Shader : public QOpenGLExtraFunctions
    {
        using Callback = std::function<void(QOpenGLContext*, QOpenGLShaderProgram*)>;

      public:
        Shader(ShaderType Type, const QString& Name);

        void Initialize();
        bool Bind();
        void Release();
        void SetCallbackBeforeLinking(const Callback& Callback);

        void AddPath(QOpenGLShader::ShaderTypeBit Type, const QString& Path);

        QString GetName() const;
        ShaderType GetShaderType() const;

        static QString GetShaderTypeString(QOpenGLShader::ShaderTypeBit Type);

        template<typename T>
        void SetUniformValue(const QString& Name, T Value)
        {
            const auto Location = mProgram->uniformLocation(Name);

            if (0 <= Location)
            {
                mProgram->setUniformValue(Location, Value);
            }
            else
            {
                LOG_FATAL("Shader::SetUniformValue[{}]: Uniform location '{}' could not be found.", mName.toStdString(), Name.toStdString());
            }
        }

        template<typename T>
        void SetUniformValueArray(const QString& Name, const QVector<T>& Values)
        {
            const auto Location = mProgram->uniformLocation(Name);

            if (0 <= Location)
            {
                mProgram->setUniformValueArray(Location, Values.constData(), Values.size());
            }
            else
            {
                LOG_FATAL("Shader::SetUniformValueArray[{}]: Uniform location '{}' could not be found.", mName.toStdString(), Name.toStdString());
            }
        }

        void SetUniformValueArray(const QString& Name, const GLfloat* Values, int Count, int TupleSize)
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
        Callback mCallback{ nullptr };
        QOpenGLShaderProgram* mProgram;
        std::map<QOpenGLShader::ShaderTypeBit, QString> mPaths;

        const ShaderType mShaderType;

        const QString mName;
    };
}
