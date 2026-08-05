#include "OpenGLStateGuard.h"

Canavar::Engine::OpenGLStateGuard::OpenGLStateGuard(QOpenGLFunctions_4_5_Core *pOpenGLFunctions)
    : mGl(pOpenGLFunctions)
{
    SaveState();
}

Canavar::Engine::OpenGLStateGuard::~OpenGLStateGuard()
{
    RestoreState();
}

void Canavar::Engine::OpenGLStateGuard::SaveState()
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
    GLint PolygonMode[2]{};
    mGl->glGetIntegerv(GL_POLYGON_MODE, PolygonMode);
    mRasterizer.PolygonMode = static_cast<GLenum>(PolygonMode[0]);
    mRasterizer.DepthClampEnabled = mGl->glIsEnabled(GL_DEPTH_CLAMP);
    mRasterizer.RasterizerDiscardEnabled = mGl->glIsEnabled(GL_RASTERIZER_DISCARD);

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

    // Primitive
    mPrimitive.LineSmoothEnabled = mGl->glIsEnabled(GL_LINE_SMOOTH);
    mGl->glGetIntegerv(GL_LINE_SMOOTH_HINT, reinterpret_cast<GLint *>(&mPrimitive.LineSmoothHint));
    mGl->glGetFloatv(GL_LINE_WIDTH, &mPrimitive.LineWidth);
    mPrimitive.ProgramPointSizeEnabled = mGl->glIsEnabled(GL_PROGRAM_POINT_SIZE);
    mPrimitive.PolygonSmoothEnabled = mGl->glIsEnabled(GL_POLYGON_SMOOTH);

    // Scissor
    mScissor.TestEnabled = mGl->glIsEnabled(GL_SCISSOR_TEST);
    mGl->glGetIntegerv(GL_SCISSOR_BOX, mScissor.Box);

    // Program
    mGl->glGetIntegerv(GL_CURRENT_PROGRAM, &mProgram.CurrentProgram);
    mGl->glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint *>(&mProgram.ActiveTexture));

    // Sample
    mSample.MultisampleEnabled = mGl->glIsEnabled(GL_MULTISAMPLE);
    mSample.AlphaToCoverageEnabled = mGl->glIsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
    mSample.AlphaToOneEnabled = mGl->glIsEnabled(GL_SAMPLE_ALPHA_TO_ONE);
    mGl->glGetFloatv(GL_SAMPLE_COVERAGE_VALUE, &mSample.CoverageValue);
    mGl->glGetBooleanv(GL_SAMPLE_COVERAGE_INVERT, &mSample.CoverageInvert);

    // Pixel Store
    mGl->glGetIntegerv(GL_PACK_ALIGNMENT, &mPixelStore.PackAlignment);
    mGl->glGetIntegerv(GL_PACK_ROW_LENGTH, &mPixelStore.PackRowLength);
    mGl->glGetIntegerv(GL_PACK_SKIP_ROWS, &mPixelStore.PackSkipRows);
    mGl->glGetIntegerv(GL_PACK_SKIP_PIXELS, &mPixelStore.PackSkipPixels);
    mGl->glGetIntegerv(GL_UNPACK_ALIGNMENT, &mPixelStore.UnpackAlignment);
    mGl->glGetIntegerv(GL_UNPACK_ROW_LENGTH, &mPixelStore.UnpackRowLength);
    mGl->glGetIntegerv(GL_UNPACK_SKIP_ROWS, &mPixelStore.UnpackSkipRows);
    mGl->glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &mPixelStore.UnpackSkipPixels);

    // Misc
    mMisc.SeamlessCubeMapEnabled = mGl->glIsEnabled(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    mMisc.DitherEnabled = mGl->glIsEnabled(GL_DITHER);
}

void Canavar::Engine::OpenGLStateGuard::RestoreState()
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
    mGl->glPolygonMode(GL_FRONT_AND_BACK, mRasterizer.PolygonMode);
    mRasterizer.DepthClampEnabled ? mGl->glEnable(GL_DEPTH_CLAMP) : mGl->glDisable(GL_DEPTH_CLAMP);
    mRasterizer.RasterizerDiscardEnabled ? mGl->glEnable(GL_RASTERIZER_DISCARD) : mGl->glDisable(GL_RASTERIZER_DISCARD);

    // Stencil
    mStencil.TestEnabled ? mGl->glEnable(GL_STENCIL_TEST) : mGl->glDisable(GL_STENCIL_TEST);
    mGl->glStencilFunc(mStencil.Func, mStencil.Ref, mStencil.ValueMask);
    mGl->glStencilMask(mStencil.WriteMask);
    mGl->glStencilOp(mStencil.Fail, mStencil.PassDepthFail, mStencil.PassDepthPass);

    // Color
    mGl->glColorMask(mColor.Mask[0], mColor.Mask[1], mColor.Mask[2], mColor.Mask[3]);
    mGl->glClearColor(mColor.ClearColor[0], mColor.ClearColor[1], mColor.ClearColor[2], mColor.ClearColor[3]);

    // Primitive
    mPrimitive.LineSmoothEnabled ? mGl->glEnable(GL_LINE_SMOOTH) : mGl->glDisable(GL_LINE_SMOOTH);
    mGl->glHint(GL_LINE_SMOOTH_HINT, mPrimitive.LineSmoothHint);
    mGl->glLineWidth(mPrimitive.LineWidth);
    mPrimitive.ProgramPointSizeEnabled ? mGl->glEnable(GL_PROGRAM_POINT_SIZE) : mGl->glDisable(GL_PROGRAM_POINT_SIZE);
    mPrimitive.PolygonSmoothEnabled ? mGl->glEnable(GL_POLYGON_SMOOTH) : mGl->glDisable(GL_POLYGON_SMOOTH);

    // Scissor
    mScissor.TestEnabled ? mGl->glEnable(GL_SCISSOR_TEST) : mGl->glDisable(GL_SCISSOR_TEST);
    mGl->glScissor(mScissor.Box[0], mScissor.Box[1], mScissor.Box[2], mScissor.Box[3]);

    // Program
    mGl->glUseProgram(mProgram.CurrentProgram);
    mGl->glActiveTexture(mProgram.ActiveTexture);

    // Sample
    mSample.MultisampleEnabled ? mGl->glEnable(GL_MULTISAMPLE) : mGl->glDisable(GL_MULTISAMPLE);
    mSample.AlphaToCoverageEnabled ? mGl->glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE) : mGl->glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    mSample.AlphaToOneEnabled ? mGl->glEnable(GL_SAMPLE_ALPHA_TO_ONE) : mGl->glDisable(GL_SAMPLE_ALPHA_TO_ONE);
    mGl->glSampleCoverage(mSample.CoverageValue, mSample.CoverageInvert);

    // Pixel Store
    mGl->glPixelStorei(GL_PACK_ALIGNMENT, mPixelStore.PackAlignment);
    mGl->glPixelStorei(GL_PACK_ROW_LENGTH, mPixelStore.PackRowLength);
    mGl->glPixelStorei(GL_PACK_SKIP_ROWS, mPixelStore.PackSkipRows);
    mGl->glPixelStorei(GL_PACK_SKIP_PIXELS, mPixelStore.PackSkipPixels);
    mGl->glPixelStorei(GL_UNPACK_ALIGNMENT, mPixelStore.UnpackAlignment);
    mGl->glPixelStorei(GL_UNPACK_ROW_LENGTH, mPixelStore.UnpackRowLength);
    mGl->glPixelStorei(GL_UNPACK_SKIP_ROWS, mPixelStore.UnpackSkipRows);
    mGl->glPixelStorei(GL_UNPACK_SKIP_PIXELS, mPixelStore.UnpackSkipPixels);

    // Misc
    mMisc.SeamlessCubeMapEnabled ? mGl->glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS) : mGl->glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    mMisc.DitherEnabled ? mGl->glEnable(GL_DITHER) : mGl->glDisable(GL_DITHER);
}

void Canavar::Engine::OpenGLStateGuard::ApplyDefaultState()
{
    // Depth
    mGl->glEnable(GL_DEPTH_TEST);
    mGl->glDepthFunc(GL_LESS);
    mGl->glDepthMask(GL_TRUE);
    mGl->glDepthRange(0.0, 1.0);

    // Blend
    mGl->glDisable(GL_BLEND);
    mGl->glBlendFunc(GL_ONE, GL_ZERO);
    mGl->glBlendEquation(GL_FUNC_ADD);
    mGl->glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Rasterizer
    mGl->glDisable(GL_CULL_FACE);
    mGl->glCullFace(GL_BACK);
    mGl->glFrontFace(GL_CCW);
    mGl->glDisable(GL_POLYGON_OFFSET_FILL);
    mGl->glPolygonOffset(0.0f, 0.0f);
    mGl->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mGl->glDisable(GL_DEPTH_CLAMP);
    mGl->glDisable(GL_RASTERIZER_DISCARD);

    // Stencil
    mGl->glDisable(GL_STENCIL_TEST);
    mGl->glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
    mGl->glStencilMask(0xFFFFFFFF);
    mGl->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    // Color
    mGl->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    mGl->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Primitive
    mGl->glDisable(GL_LINE_SMOOTH);
    mGl->glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    mGl->glLineWidth(1.0f);
    mGl->glDisable(GL_PROGRAM_POINT_SIZE);
    mGl->glDisable(GL_POLYGON_SMOOTH);

    // Scissor
    mGl->glDisable(GL_SCISSOR_TEST);
    mGl->glScissor(0, 0, 800, 600); // Default scissor box

    // Sample
    mGl->glEnable(GL_MULTISAMPLE);
    mGl->glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    mGl->glDisable(GL_SAMPLE_ALPHA_TO_ONE);
    mGl->glSampleCoverage(1.0f, GL_FALSE);

    // Pixel Store
    mGl->glPixelStorei(GL_PACK_ALIGNMENT, 4);
    mGl->glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    mGl->glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    mGl->glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    mGl->glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    mGl->glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    mGl->glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    mGl->glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    // Misc
    mGl->glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    mGl->glEnable(GL_DITHER);
}
