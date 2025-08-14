#include "ShadowMappingFramebuffer.h"

Canavar::Engine::ShadowMappingFramebuffer::ShadowMappingFramebuffer(int width, int height)
    : mWidth(width)
    , mHeight(height)
{
    initializeOpenGLFunctions();
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    glGenTextures(1, &mDepthMap);
    glBindTexture(GL_TEXTURE_2D, mDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    constexpr float BORDER_COLOR[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        CGE_EXIT_FAILURE("ShadowMappingFramebuffer::ShadowMappingFramebuffer: Framebuffer could not be created!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Canavar::Engine::ShadowMappingFramebuffer::~ShadowMappingFramebuffer()
{
    if (mFBO)
    {
        glDeleteFramebuffers(1, &mFBO);
        mFBO = 0;
    }

    if (mDepthMap)
    {
        glDeleteTextures(1, &mDepthMap);
        mDepthMap = 0;
    }
}

void Canavar::Engine::ShadowMappingFramebuffer::Bind()
{
    if (mFBO)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glViewport(0, 0, mWidth, mHeight);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

GLuint Canavar::Engine::ShadowMappingFramebuffer::GetDepthMap() const
{
    return mDepthMap;
}
