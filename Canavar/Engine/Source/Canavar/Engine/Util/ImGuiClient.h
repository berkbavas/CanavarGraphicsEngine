#pragma once

namespace Canavar::Engine
{
    class ImGuiClient
    {
      public:
        virtual ~ImGuiClient() = default;
        virtual void Draw() = 0;
    };
}