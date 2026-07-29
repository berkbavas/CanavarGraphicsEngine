#pragma once

#include "Canavar/Engine/Util/Logger.h"

#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>

namespace Canavar::Engine
{
    class Shader : public QOpenGLExtraFunctions
    {
        using Callback = std::function<void(QOpenGLContext*, QOpenGLShaderProgram*)>;
        static constexpr int MINIMUM_VALID_LOCATION{ 0 }; // Minimum valid uniform location (0 or higher). Negative values indicate an invalid location.

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
            const auto Location = GetUniformLocation(Name);
            mProgram->setUniformValue(Location, Value);
        }

        template<typename T>
        void SetUniformArray(const QString& Name, const QVector<T>& Values)
        {
            const auto Location = GetUniformLocation(Name);
            mProgram->setUniformValueArray(Location, Values.constData(), Values.size());
        }

        void SetUniformArray(const QString& Name, const GLfloat* pValues, int Count, int TupleSize)
        {
            const auto Location = GetUniformLocation(Name);
            mProgram->setUniformValueArray(Location, pValues, Count, TupleSize);
        }

        void SetSampler(const QString& Name, GLuint Unit, GLuint TextureId, GLuint Target = GL_TEXTURE_2D);

      private:
        int GetUniformLocation(const QString& Name);

        QString mName;
        std::unique_ptr<QOpenGLShaderProgram> mProgram;
        std::map<QOpenGLShader::ShaderTypeBit, QString> mPaths;
        std::unordered_map<QString, int> mUniformLocations;
        Callback mCallback{ nullptr };
    };

    using ShaderPtr = std::unique_ptr<Shader>;
}
