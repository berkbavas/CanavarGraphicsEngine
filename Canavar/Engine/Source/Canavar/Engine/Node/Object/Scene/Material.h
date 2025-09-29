#pragma once

#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QMap>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QVector3D>

namespace Canavar::Engine
{
    enum class TextureType
    {
        BaseColor,
        Metallic,
        Roughness,
        AmbientOcclusion,
        Normal
    };

    class Material : protected QOpenGLExtraFunctions
    {
        DISABLE_COPY(Material);

      public:
        Material();
        ~Material();

        bool LoadTextureFromPath(TextureType Type, const QString &Path, int Components);
        bool LoadTextureFromImage(TextureType Type, const QImage &Image, int Components);

        GLuint GetTexture(TextureType Type) const;

        bool HasTextureBaseColor() const;
        bool HasTextureMetallic() const;
        bool HasTextureRoughness() const;
        bool HasTextureAmbientOcclusion() const;
        bool HasTextureNormal() const;

        int GetNumberOfTextures() const;

      private:
        QMap<TextureType, GLuint> mTextures;

        DEFINE_MEMBER(float, Opacity, 1.0f);
    };

    using MaterialPtr = std::shared_ptr<Material>;
}