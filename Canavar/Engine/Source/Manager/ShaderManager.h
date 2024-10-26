#pragma once

#include "Manager/Manager.h"
#include "Util/Shader.h"

#include <map>

namespace Canavar::Engine
{
    enum class ShaderType
    {
        Model,
    };

    class ShaderManager : public Manager
    {
      public:
        explicit ShaderManager(QObject* parent = nullptr);

        void Initialize();

        Shader* GetShader(ShaderType type);

      private:
        std::map<ShaderType, Shader*> mShaders;

        Shader* mModelShader{ nullptr };
    };
}