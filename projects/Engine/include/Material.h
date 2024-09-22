#pragma once

#include <QMap>
#include <QObject>
#include <QOpenGLTexture>

namespace Canavar {
    namespace Engine {
        class Material : public QObject
        {
        public:
            Material();
            virtual ~Material();

            enum class TextureType //
            {
                Ambient,
                Diffuse,
                Specular,
                Normal
            };

            void Insert(TextureType type, QOpenGLTexture* texture);
            QOpenGLTexture* Get(TextureType type);
            int GetNumberOfTextures();

        private:
            QMap<TextureType, QOpenGLTexture*> mTextures;
        };
    } // namespace Engine
} // namespace Canavar
