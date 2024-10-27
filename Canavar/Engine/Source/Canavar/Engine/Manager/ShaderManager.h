#pragma once

#include "Canavar/Engine/Manager/Manager.h"
#include "Canavar/Engine/Util/Shader.h"

#include <map>

namespace Canavar::Engine
{
    enum class ShaderType
    {
        Model,
        Sky,
        Terrain,
        Blur,
        PostProcess
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
        Shader* mSkyShader{ nullptr };
        Shader* mTerrainShader{ nullptr };
        Shader* mBlurShader{ nullptr };
        Shader* mPostProcessShader{ nullptr };
    };
}