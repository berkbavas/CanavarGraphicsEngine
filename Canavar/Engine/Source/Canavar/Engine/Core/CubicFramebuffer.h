#pragma once

#include <memory>

#include <QOpenGLFunctions_4_5_Core>

namespace Canavar::Engine
{
    class CubicFramebuffer : protected QOpenGLFunctions_4_5_Core
    {
      public:
        explicit CubicFramebuffer(int Width, int Height);
        CubicFramebuffer(const CubicFramebuffer &) = delete;
        CubicFramebuffer &operator=(const CubicFramebuffer &) = delete;
        ~CubicFramebuffer();

        GLuint GetDepthMap() const;
        GLuint GetFramebuffer() const;
        int GetWidth() const;
        int GetHeight() const;

        void Clear();
        void Bind();
        void Unbind();

      private:
        GLuint mFramebuffer{ 0 };
        GLuint mDepthMap{ 0 };
        

        int mWidth{ 1 };
        int mHeight{ 1 };
    };

    using CubicFramebufferPtr = std::unique_ptr<CubicFramebuffer>;
}