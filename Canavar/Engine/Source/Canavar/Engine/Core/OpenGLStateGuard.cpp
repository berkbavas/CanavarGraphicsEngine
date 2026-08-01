#include "OpenGLStateGuard.h"

Canavar::Engine::OpenGLStateGuard::OpenGLStateGuard(QOpenGLFunctions_4_5_Core *pOpenGLFunctions)
    : mGl(pOpenGLFunctions)
{
    // Framebuffer
    mGl->glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mFramebuffer.Binding);
    mGl->glGetIntegerv(GL_VIEWPORT, mFramebuffer.Viewport);

    // Depth
    mDepth.TestEnabled = mGl->glIsEnabled(GL_DEPTH_TEST);
    mGl->glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&mDepth.Func));
    mGl->glGetBooleanv(GL_DEPTH_WRITEMASK, &mDepth.Mask);
    GLdouble DepthRange[2]{};
    mGl->glGetDoublev(GL_DEPTH_RANGE, DepthRange);
    mDepth.RangeNear = DepthRange[0];
    mDepth.RangeFar = DepthRange[1];

    // Blend
    mBlend.Enabled = mGl->glIsEnabled(GL_BLEND);
    mGl->glGetIntegerv(GL_BLEND_SRC_RGB, reinterpret_cast<GLint *>(&mBlend.SrcRGB));
    mGl->glGetIntegerv(GL_BLEND_DST_RGB, reinterpret_cast<GLint *>(&mBlend.DstRGB));
    mGl->glGetIntegerv(GL_BLEND_SRC_ALPHA, reinterpret_cast<GLint *>(&mBlend.SrcAlpha));
    mGl->glGetIntegerv(GL_BLEND_DST_ALPHA, reinterpret_cast<GLint *>(&mBlend.DstAlpha));
    mGl->glGetIntegerv(GL_BLEND_EQUATION_RGB, reinterpret_cast<GLint *>(&mBlend.EquationRGB));
    mGl->glGetIntegerv(GL_BLEND_EQUATION_ALPHA, reinterpret_cast<GLint *>(&mBlend.EquationAlpha));
    mGl->glGetFloatv(GL_BLEND_COLOR, mBlend.Color);

    // Rasterizer
    mRasterizer.CullFaceEnabled = mGl->glIsEnabled(GL_CULL_FACE);
    mGl->glGetIntegerv(GL_CULL_FACE_MODE, reinterpret_cast<GLint *>(&mRasterizer.CullFaceMode));
    mGl->glGetIntegerv(GL_FRONT_FACE, reinterpret_cast<GLint *>(&mRasterizer.FrontFace));
    mRasterizer.PolygonOffsetFillEnabled = mGl->glIsEnabled(GL_POLYGON_OFFSET_FILL);
    mGl->glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &mRasterizer.PolygonOffsetFactor);
    mGl->glGetFloatv(GL_POLYGON_OFFSET_UNITS, &mRasterizer.PolygonOffsetUnits);

    // Stencil
    mStencil.TestEnabled = mGl->glIsEnabled(GL_STENCIL_TEST);
    mGl->glGetIntegerv(GL_STENCIL_FUNC, reinterpret_cast<GLint *>(&mStencil.Func));
    mGl->glGetIntegerv(GL_STENCIL_REF, &mStencil.Ref);
    mGl->glGetIntegerv(GL_STENCIL_VALUE_MASK, reinterpret_cast<GLint *>(&mStencil.ValueMask));
    mGl->glGetIntegerv(GL_STENCIL_WRITEMASK, reinterpret_cast<GLint *>(&mStencil.WriteMask));
    mGl->glGetIntegerv(GL_STENCIL_FAIL, reinterpret_cast<GLint *>(&mStencil.Fail));
    mGl->glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, reinterpret_cast<GLint *>(&mStencil.PassDepthFail));
    mGl->glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, reinterpret_cast<GLint *>(&mStencil.PassDepthPass));

    // Color
    mGl->glGetBooleanv(GL_COLOR_WRITEMASK, mColor.Mask);
    mGl->glGetFloatv(GL_COLOR_CLEAR_VALUE, mColor.ClearColor);
}

Canavar::Engine::OpenGLStateGuard::~OpenGLStateGuard()
{
    // Framebuffer
    mGl->glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer.Binding);
    mGl->glViewport(mFramebuffer.Viewport[0], mFramebuffer.Viewport[1], mFramebuffer.Viewport[2], mFramebuffer.Viewport[3]);

    // Depth
    mDepth.TestEnabled ? mGl->glEnable(GL_DEPTH_TEST) : mGl->glDisable(GL_DEPTH_TEST);
    mGl->glDepthFunc(mDepth.Func);
    mGl->glDepthMask(mDepth.Mask);
    mGl->glDepthRange(mDepth.RangeNear, mDepth.RangeFar);

    // Blend
    mBlend.Enabled ? mGl->glEnable(GL_BLEND) : mGl->glDisable(GL_BLEND);
    mGl->glBlendFuncSeparate(mBlend.SrcRGB, mBlend.DstRGB, mBlend.SrcAlpha, mBlend.DstAlpha);
    mGl->glBlendEquationSeparate(mBlend.EquationRGB, mBlend.EquationAlpha);
    mGl->glBlendColor(mBlend.Color[0], mBlend.Color[1], mBlend.Color[2], mBlend.Color[3]);

    // Rasterizer
    mRasterizer.CullFaceEnabled ? mGl->glEnable(GL_CULL_FACE) : mGl->glDisable(GL_CULL_FACE);
    mGl->glCullFace(mRasterizer.CullFaceMode);
    mGl->glFrontFace(mRasterizer.FrontFace);
    mRasterizer.PolygonOffsetFillEnabled ? mGl->glEnable(GL_POLYGON_OFFSET_FILL) : mGl->glDisable(GL_POLYGON_OFFSET_FILL);
    mGl->glPolygonOffset(mRasterizer.PolygonOffsetFactor, mRasterizer.PolygonOffsetUnits);

    // Stencil
    mStencil.TestEnabled ? mGl->glEnable(GL_STENCIL_TEST) : mGl->glDisable(GL_STENCIL_TEST);
    mGl->glStencilFunc(mStencil.Func, mStencil.Ref, mStencil.ValueMask);
    mGl->glStencilMask(mStencil.WriteMask);
    mGl->glStencilOp(mStencil.Fail, mStencil.PassDepthFail, mStencil.PassDepthPass);

    // Color
    mGl->glColorMask(mColor.Mask[0], mColor.Mask[1], mColor.Mask[2], mColor.Mask[3]);
    mGl->glClearColor(mColor.ClearColor[0], mColor.ClearColor[1], mColor.ClearColor[2], mColor.ClearColor[3]);
}
