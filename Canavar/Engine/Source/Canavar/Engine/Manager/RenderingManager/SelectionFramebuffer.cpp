#include "SelectionFramebuffer.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::SelectionFramebuffer::SelectionFramebuffer(int width, int height)
    : mWidth(width)
    , mHeight(height)
{
    initializeOpenGLFunctions();

    glGenFramebuffers(1, &mFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);

    glGenTextures(1, &mTextures[0]);
    glBindTexture(GL_TEXTURE_2D, mTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, mWidth, mHeight, 0, GL_RGBA_INTEGER, GL_INT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextures[0], 0);

    glGenTextures(1, &mTextures[1]);
    glBindTexture(GL_TEXTURE_2D, mTextures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, mWidth, mHeight, 0, GL_RGBA_INTEGER, GL_INT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mTextures[1], 0);

    glGenRenderbuffers(1, &mRenderBufferObject);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferObject);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferObject);

    constexpr GLuint ATTACHMENTS[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, ATTACHMENTS);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_FATAL("SelectionFramebuffer::SelectionFramebuffer: Framebuffer could not be created!");
    }

    LOG_FATAL("SelectionFramebuffer::SelectionFramebuffer: Framebuffer created: ({}, {})", mWidth, mHeight);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
Canavar::Engine::SelectionFramebuffer::~SelectionFramebuffer()
{
    if (mFramebuffer)
    {
        glDeleteFramebuffers(1, &mFramebuffer);
    }

    if (mTextures[0])
    {
        glDeleteTextures(1, &mTextures[0]);
    }

    if (mTextures[1])
    {
        glDeleteTextures(1, &mTextures[1]);
    }

    if (mRenderBufferObject)
    {
        glDeleteBuffers(1, &mFramebuffer);
    }
}

void Canavar::Engine::SelectionFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
}

void Canavar::Engine::SelectionFramebuffer::Release()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}