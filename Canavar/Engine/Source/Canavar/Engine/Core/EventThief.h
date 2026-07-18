#pragma once

namespace Canavar::Engine
{
    class EventThief
    {
      public:
        EventThief() = default;
        virtual ~EventThief() = default;

        virtual bool WantsKeyboardCapture() const = 0;
        virtual bool WantsMouseCapture() const = 0;
    };
}