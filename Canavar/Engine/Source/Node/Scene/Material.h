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
        void SetTexture(TextureType type, QOpenGLTexture* texture) { mTextures.insert(type, texture); }

        QOpenGLTexture* GetTexture(TextureType type) { return mTextures.value(type, nullptr); }

        int GetNumberOfTextures() { return mTextures.size(); }

      private:
        QMap<TextureType, QOpenGLTexture*> mTextures;
    };

    using MaterialPtr = std::shared_ptr<Material>;
}