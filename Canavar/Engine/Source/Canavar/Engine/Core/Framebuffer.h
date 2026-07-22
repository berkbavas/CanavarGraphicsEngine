#pragma once

#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFunctions_4_5_Core>

namespace Canavar::Engine
{
    class Framebuffer : protected QOpenGLFunctions_4_5_Core
    {
      public:
        explicit Framebuffer(int Width, int Height, const QOpenGLFramebufferObjectFormat& Format, const QVector<GLenum>& ExtraColorAttachments = {});
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        ~Framebuffer() = default;

        void Resize(int Width, int Height);

        void Bind();
        void Unbind();
        void Release();

        void BlitDepthTo(Framebuffer* TargetFramebuffer);
        void BlitColorBufferTo(Framebuffer* TargetFramebuffer, GLuint Attachment);
        void ClearDepthBuffer();

        QOpenGLFramebufferObject* GetFramebufferObject() const;
        const QOpenGLFramebufferObjectFormat& GetFramebufferFormat() const;
        GLuint GetTexture() const;
        QList<GLuint> GetTextures() const;
        GLuint GetHandle() const;
        int GetWidth() const;
        int GetHeight() const;
        QRect GetViewport() const;

      private:
        void AddColorAttachment(GLenum InternalFormat);

        std::unique_ptr<QOpenGLFramebufferObject> mFramebuffer{ nullptr };
        QOpenGLFramebufferObjectFormat mFramebufferFormat;
        std::map<GLuint, GLenum> mExtraColorAttachments; // Maps color attachment indices to their internal formats
        std::vector<GLenum> mDrawBuffers;                // List of draw buffers for glDrawBuffers

        int mNumColorAttachments{ 0 };

        int mWidth{ 0 };
        int mHeight{ 0 };

        static constexpr GLenum FBO_ATTACHMENTS[8] = //
            {
                GL_COLOR_ATTACHMENT0, //
                GL_COLOR_ATTACHMENT1, //
                GL_COLOR_ATTACHMENT2, //
                GL_COLOR_ATTACHMENT3, //
                GL_COLOR_ATTACHMENT4, //
                GL_COLOR_ATTACHMENT5, //
                GL_COLOR_ATTACHMENT6, //
                GL_COLOR_ATTACHMENT7, //
            };
    };

    using FramebufferPtr = std::unique_ptr<Framebuffer>;
}