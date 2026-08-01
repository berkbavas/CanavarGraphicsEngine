#pragma once

#include <QOpenGLFunctions_4_5_Core>

namespace Canavar::Engine
{
    class OpenGLStateGuard
    {
      public:
        explicit OpenGLStateGuard(QOpenGLFunctions_4_5_Core *pOpenGLFunctions);
        OpenGLStateGuard(const OpenGLStateGuard &) = delete;
        OpenGLStateGuard &operator=(const OpenGLStateGuard &) = delete;
        ~OpenGLStateGuard();

      private:
        struct FramebufferState
        {
            GLint Binding{ 0 };
            GLint Viewport[4]{ 0, 0, 0, 0 };
        };

        struct DepthState
        {
            GLboolean TestEnabled{ GL_FALSE };
            GLenum Func{ GL_LESS };
            GLboolean Mask{ GL_TRUE };
            GLdouble RangeNear{ 0.0 };
            GLdouble RangeFar{ 1.0 };
        };

        struct BlendState
        {
            GLboolean Enabled{ GL_FALSE };
            GLenum SrcRGB{ GL_ONE };
            GLenum DstRGB{ GL_ZERO };
            GLenum SrcAlpha{ GL_ONE };
            GLenum DstAlpha{ GL_ZERO };
            GLenum EquationRGB{ GL_FUNC_ADD };
            GLenum EquationAlpha{ GL_FUNC_ADD };
            GLfloat Color[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
        };

        struct RasterizerState
        {
            GLboolean CullFaceEnabled{ GL_FALSE };
            GLenum CullFaceMode{ GL_BACK };
            GLenum FrontFace{ GL_CCW };
            GLboolean PolygonOffsetFillEnabled{ GL_FALSE };
            GLfloat PolygonOffsetFactor{ 0.0f };
            GLfloat PolygonOffsetUnits{ 0.0f };
        };

        struct StencilState
        {
            GLboolean TestEnabled{ GL_FALSE };
            GLenum Func{ GL_ALWAYS };
            GLint Ref{ 0 };
            GLuint ValueMask{ 0xFFFFFFFFu };
            GLuint WriteMask{ 0xFFFFFFFFu };
            GLenum Fail{ GL_KEEP };
            GLenum PassDepthFail{ GL_KEEP };
            GLenum PassDepthPass{ GL_KEEP };
        };

        struct ColorState
        {
            GLboolean Mask[4]{ GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE };
            GLfloat ClearColor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
        };

        QOpenGLFunctions_4_5_Core *mGl{ nullptr };

        FramebufferState mFramebuffer;
        DepthState mDepth;
        BlendState mBlend;
        RasterizerState mRasterizer;
        StencilState mStencil;
        ColorState mColor;
    };
}