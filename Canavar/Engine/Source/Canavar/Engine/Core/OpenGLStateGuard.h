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
            GLenum PolygonMode{ GL_FILL };
            GLboolean DepthClampEnabled{ GL_FALSE };
            GLboolean RasterizerDiscardEnabled{ GL_FALSE };
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

        struct PrimitiveState
        {
            GLboolean LineSmoothEnabled{ GL_FALSE };
            GLenum LineSmoothHint{ GL_DONT_CARE };
            GLfloat LineWidth{ 1.0f };
            GLboolean ProgramPointSizeEnabled{ GL_FALSE };
            GLboolean PolygonSmoothEnabled{ GL_FALSE };
        };

        struct ScissorState
        {
            GLboolean TestEnabled{ GL_FALSE };
            GLint Box[4]{ 0, 0, 0, 0 };
        };

        struct ProgramState
        {
            GLint CurrentProgram{ 0 };
            GLenum ActiveTexture{ GL_TEXTURE0 };
        };

        struct SampleState
        {
            GLboolean MultisampleEnabled{ GL_TRUE };
            GLboolean AlphaToCoverageEnabled{ GL_FALSE };
            GLboolean AlphaToOneEnabled{ GL_FALSE };
            GLfloat CoverageValue{ 1.0f };
            GLboolean CoverageInvert{ GL_FALSE };
        };

        struct PixelStoreState
        {
            GLint PackAlignment{ 4 };
            GLint PackRowLength{ 0 };
            GLint PackSkipRows{ 0 };
            GLint PackSkipPixels{ 0 };
            GLint UnpackAlignment{ 4 };
            GLint UnpackRowLength{ 0 };
            GLint UnpackSkipRows{ 0 };
            GLint UnpackSkipPixels{ 0 };
        };

        struct MiscState
        {
            GLboolean SeamlessCubeMapEnabled{ GL_FALSE };
            GLboolean DitherEnabled{ GL_TRUE };
        };

        QOpenGLFunctions_4_5_Core *mGl{ nullptr };

        FramebufferState mFramebuffer;
        DepthState mDepth;
        BlendState mBlend;
        RasterizerState mRasterizer;
        StencilState mStencil;
        ColorState mColor;
        PrimitiveState mPrimitive;
        ScissorState mScissor;
        ProgramState mProgram;
        SampleState mSample;
        PixelStoreState mPixelStore;
        MiscState mMisc;
    };
}