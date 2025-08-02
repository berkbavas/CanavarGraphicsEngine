#pragma once

#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QMap>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>

namespace Canavar::Engine
{
    enum class TextureType
    {
        BaseColor,
        Normal
    };

    class Material : protected QOpenGLExtraFunctions
    {
        DISABLE_COPY(Material);

      public:
        Material();
        ~Material();

        void LoadTexture(TextureType type, const QImage& image);
        GLuint GetTexture(TextureType type);

        bool HasTextureBaseColor() const;
        bool HasTextureNormal() const;

        int GetNumberOfTextures() const;

      private:
        QMap<TextureType, GLuint> mTextures;
    };

    using MaterialPtr = std::shared_ptr<Material>;
}