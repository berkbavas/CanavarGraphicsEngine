#pragma once

#include <memory>

#include <QMap>
#include <QOpenGLTexture>

namespace Canavar::Engine
{
    enum class TextureType
    {
        Ambient,
        Diffuse,
        Specular,
        Normal
    };

    class Material
    {
      public:
        void SetTexture(TextureType type, QOpenGLTexture* texture);

        QOpenGLTexture* GetTexture(TextureType type);

        int GetNumberOfTextures();

      private:
        QMap<TextureType, QOpenGLTexture*> mTextures;
    };

    using MaterialPtr = std::shared_ptr<Material>;
}