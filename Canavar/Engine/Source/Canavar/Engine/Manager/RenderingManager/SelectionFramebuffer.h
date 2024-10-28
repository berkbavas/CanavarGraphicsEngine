#pragma once

#include "Canavar/Engine/Util/Macros.h"

#include <QOpenGLExtraFunctions>

namespace Canavar::Engine
{
    class SelectionFramebuffer : protected QOpenGLExtraFunctions
    {
        DISABLE_COPY(SelectionFramebuffer);

      public:
        SelectionFramebuffer(int width, int height);
        ~SelectionFramebuffer();

        void Bind();
        void Release();

      private:
        // OpenGL Handles
        GLuint mFramebuffer{ 0 };
        GLuint mRenderBufferObject{ 0 };
        GLuint mTextures[2];

        const int mWidth;
        const int mHeight;
    };
}
