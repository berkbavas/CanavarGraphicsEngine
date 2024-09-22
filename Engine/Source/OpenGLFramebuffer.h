#pragma once

#include <QOpenGLExtraFunctions>

namespace Canavar {
    namespace Engine {
        class OpenGLFramebuffer : protected QOpenGLExtraFunctions
        {
        public:
            OpenGLFramebuffer();

            void Init();
            void Create(int width, int height);
            void Destroy();

            void Bind();
            void Release();

        private:
            bool mCreated;
            unsigned int mFBO;
            unsigned int mRBO;
            unsigned int mTextures[2];

            GLuint mColorAttachments[2];
        };
    } // namespace Engine
} // namespace Canavar
