#pragma once

namespace Canavar::Engine
{
    class RenderingContext
    {
      public:
        explicit RenderingContext() = default;
        virtual ~RenderingContext() = default;

        virtual void MakeCurrent() = 0;
        virtual void DoneCurrent() = 0;

        virtual float GetDevicePixelRatio() const = 0;
    };
}