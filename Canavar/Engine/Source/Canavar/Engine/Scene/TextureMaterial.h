#pragma once

#include "Canavar/Engine/Core/Enums.h"
#include <memory>

#include <QMap>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>

namespace Canavar::Engine
{
    class TextureMaterial : protected QOpenGLExtraFunctions
    {
      public:
        explicit TextureMaterial(float Opacity);
        TextureMaterial(const TextureMaterial &) = delete;
        TextureMaterial &operator=(const TextureMaterial &) = delete;
        ~TextureMaterial();

        bool LoadTexture(TextureType Type, const QImage &Image, int ComponentCount = 4);

        float GetOpacity() const;
        GLuint GetTexture(TextureType Type);
        bool HasTexture(TextureType Type) const;
        int GetNumberOfTextures() const;

      private:
        QMap<TextureType, GLuint> mTextures;
        const float mOpacity;
    };

    using TextureMaterialPtr = std::shared_ptr<TextureMaterial>;
    using TextureMaterialWeakPtr = std::weak_ptr<TextureMaterial>;
}