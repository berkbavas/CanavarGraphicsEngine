#pragma once

#include "Canavar/Engine/Util/Logger.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QOpenGLFunctions_4_3_Core>

namespace Canavar::Engine
{
    class ShadowMappingFramebuffer : protected QOpenGLFunctions_4_3_Core
    {
        DISABLE_COPY(ShadowMappingFramebuffer);

      public:
        ShadowMappingFramebuffer(int width, int height);
        ~ShadowMappingFramebuffer();

        void Bind();
        GLuint GetDepthMap() const;

      private:
        GLuint mFBO{ 0 };
        GLuint mDepthMap{ 0 };

        const int mWidth;
        const int mHeight;
    };

}