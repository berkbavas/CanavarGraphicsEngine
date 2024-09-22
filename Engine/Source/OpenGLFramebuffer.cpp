#include "OpenGLFramebuffer.h"

Canavar::Engine::OpenGLFramebuffer::OpenGLFramebuffer()
    : mCreated(false)
    , mFBO(0)
    , mRBO(0)
    , mTextures{ 0, 0 }
    , mColorAttachments{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 }
{}

void Canavar::Engine::OpenGLFramebuffer::Init()
{
    initializeOpenGLFunctions();
}

void Canavar::Engine::OpenGLFramebuffer::Create(int width, int height)
{
    if (mCreated)
    {
        qCritical() << "FBO is already created!";
        return;
    }

    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    glGenTextures(1, &mTextures[0]);
    glBindTexture(GL_TEXTURE_2D, mTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, width, height, 0, GL_RGBA_INTEGER, GL_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextures[0], 0);

    glGenTextures(1, &mTextures[1]);
    glBindTexture(GL_TEXTURE_2D, mTextures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, width, height, 0, GL_RGBA_INTEGER, GL_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mTextures[1], 0);

    glGenRenderbuffers(1, &mRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRBO);

    glDrawBuffers(2, mColorAttachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qCritical() << "Framebuffer is could not be created.";
        return;
    }

    mCreated = true;
}

void Canavar::Engine::OpenGLFramebuffer::Destroy()
{
    if (mFBO)
        glDeleteFramebuffers(1, &mFBO);

    if (mTextures[0])
        glDeleteTextures(1, &mTextures[0]);

    if (mTextures[1])
        glDeleteTextures(1, &mTextures[1]);

    if (mRBO)
        glDeleteBuffers(1, &mRBO);

    mCreated = false;
}

void Canavar::Engine::OpenGLFramebuffer::Bind()
{
    if (mCreated)
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}

void Canavar::Engine::OpenGLFramebuffer::Release()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}