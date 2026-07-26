#pragma once

namespace Canavar::Engine
{
    class EventThief
    {
      public:
        EventThief() = default;
        virtual ~EventThief() = default;

        virtual bool WantCaptureKeyboard() const = 0;
        virtual bool WantCaptureMouse() const = 0;
    };
}