#include "Framebuffer.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::Framebuffer::Framebuffer(int Width, int Height, const QOpenGLFramebufferObjectFormat &Format, const QVector<GLenum> &ExtraColorAttachments)
    : mWidth(Width)
    , mHeight(Height)
    , mFramebufferFormat(Format)
{
    initializeOpenGLFunctions(); // Initialize OpenGL functions for this class

    mDrawBuffers.push_back(FBO_ATTACHMENTS[0]); // Default to the first color attachment
    mNumColorAttachments = 1;                   // Start with one color attachment

    for (const auto &InternalFormat : ExtraColorAttachments)
    {
        AddColorAttachment(InternalFormat); // Add any extra color attachments specified
    }

    Resize(Width, Height); // Create the framebuffer with the specified width and height
}

void Canavar::Engine::Framebuffer::AddColorAttachment(GLenum InternalFormat)
{
    CGE_EXIT_FAILURE_IF(mNumColorAttachments >= 8, "Framebuffer::AddColorAttachment: Cannot add more than 8 color attachments.");

    const auto NextAttachment = FBO_ATTACHMENTS[mNumColorAttachments];

    mExtraColorAttachments[NextAttachment] = InternalFormat; // Store the internal format for this attachment
    mDrawBuffers.push_back(NextAttachment);                  // Add the corresponding draw buffer
    mNumColorAttachments++;
}

void Canavar::Engine::Framebuffer::Resize(int Width, int Height)
{
    mWidth = Width;
    mHeight = Height;

    if (mFramebuffer)
    {
        mFramebuffer->release(); // Release the existing framebuffer before resizing
    }

    // Create a new framebuffer with the updated size and the same format
    mFramebuffer = std::make_unique<QOpenGLFramebufferObject>(Width, Height, mFramebufferFormat);

    // Re-add extra color attachments if any were added previously
    for (const auto &[Attachment, InternalFormat] : mExtraColorAttachments)
    {
        mFramebuffer->addColorAttachment(Width, Height, InternalFormat);
    }

    mFramebuffer->bind();
    glDrawBuffers(static_cast<GLsizei>(mDrawBuffers.size()), mDrawBuffers.data());
    mFramebuffer->release();
}

void Canavar::Engine::Framebuffer::Bind()
{
    mFramebuffer->bind();
    glDrawBuffers(static_cast<GLsizei>(mDrawBuffers.size()), mDrawBuffers.data());
}

void Canavar::Engine::Framebuffer::Unbind()
{
    Release();
}

void Canavar::Engine::Framebuffer::Release()
{
    mFramebuffer->release();
}

void Canavar::Engine::Framebuffer::ClearDepthBuffer()
{
    mFramebuffer->bind();
    glDrawBuffers(static_cast<GLsizei>(mDrawBuffers.size()), mDrawBuffers.data());
    glClear(GL_DEPTH_BUFFER_BIT);
    mFramebuffer->release();
}

QOpenGLFramebufferObject *Canavar::Engine::Framebuffer::GetFramebufferObject() const
{
    return mFramebuffer.get();
}

const QOpenGLFramebufferObjectFormat &Canavar::Engine::Framebuffer::GetFramebufferFormat() const
{
    return mFramebufferFormat;
}

GLuint Canavar::Engine::Framebuffer::GetTexture() const
{
    return mFramebuffer ? mFramebuffer->texture() : 0;
}

QList<GLuint> Canavar::Engine::Framebuffer::GetTextures() const
{
    return mFramebuffer ? mFramebuffer->textures() : QList<GLuint>();
}

GLuint Canavar::Engine::Framebuffer::GetHandle() const
{
    return mFramebuffer ? mFramebuffer->handle() : 0;
}

int Canavar::Engine::Framebuffer::GetWidth() const
{
    return mWidth;
}

int Canavar::Engine::Framebuffer::GetHeight() const
{
    return mHeight;
}

void Canavar::Engine::Framebuffer::BlitDepthTo(Framebuffer *TargetFramebuffer)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, GetHandle());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, TargetFramebuffer->GetHandle());

    glBlitFramebuffer(0,
                      0,
                      mWidth,
                      mHeight, // Source bounds
                      0,
                      0,
                      TargetFramebuffer->GetWidth(),
                      TargetFramebuffer->GetHeight(), // Destination bounds
                      GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);
}

void Canavar::Engine::Framebuffer::BlitColorBufferTo(Framebuffer *TargetFramebuffer, GLuint Attachment)
{
    QOpenGLFramebufferObject::blitFramebuffer( //
        TargetFramebuffer->GetFramebufferObject(),
        QRect(0, 0, TargetFramebuffer->GetWidth(), TargetFramebuffer->GetHeight()),
        mFramebuffer.get(),
        QRect(0, 0, mWidth, mHeight),
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR,
        Attachment - GL_COLOR_ATTACHMENT0,
        Attachment - GL_COLOR_ATTACHMENT0);
}
