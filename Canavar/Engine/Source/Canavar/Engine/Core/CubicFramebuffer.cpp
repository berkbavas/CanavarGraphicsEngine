#include "CubicFramebuffer.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::CubicFramebuffer::CubicFramebuffer(int Width, int Height)
    : mWidth(Width)
    , mHeight(Height)
{
    initializeOpenGLFunctions();

    glGenFramebuffers(1, &mFramebuffer);
    glGenTextures(1, &mDepthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthMap);

    for (unsigned int Index = 0; Index < 6; ++Index)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, //
                     0,
                     GL_DEPTH_COMPONENT,
                     Width,
                     Height,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        CGE_EXIT_FAILURE("CubicFramebuffer::CubicFramebuffer: Framebuffer is not complete.");
    }
}

Canavar::Engine::CubicFramebuffer::~CubicFramebuffer()
{
    glDeleteFramebuffers(1, &mFramebuffer);
    glDeleteTextures(1, &mDepthMap);
}

void Canavar::Engine::CubicFramebuffer::Clear()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canavar::Engine::CubicFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
}

void Canavar::Engine::CubicFramebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Canavar::Engine::CubicFramebuffer::GetDepthMap() const
{
    return mDepthMap;
}

GLuint Canavar::Engine::CubicFramebuffer::GetFramebuffer() const
{
    return mFramebuffer;
}

int Canavar::Engine::CubicFramebuffer::GetWidth() const
{
    return mWidth;
}

int Canavar::Engine::CubicFramebuffer::GetHeight() const
{
    return mHeight;
}
