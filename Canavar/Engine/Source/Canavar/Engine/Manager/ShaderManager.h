#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/Manager.h"

#include <map>

namespace Canavar::Engine
{
    class ShaderManager : public Manager
    {
      public:
        explicit ShaderManager(QObject* parent = nullptr);

        void Initialize();

        Shader* GetShader(ShaderType shaderType);

      private:
        std::map<ShaderType, Shader*> mShaders;

        Shader* mModelShader{ nullptr };
        Shader* mSkyShader{ nullptr };
        Shader* mBlurShader{ nullptr };
        Shader* mPostProcessShader{ nullptr };
        Shader* mNozzleEffect{ nullptr };
        Shader* mLineShader{ nullptr };
        Shader* mLightningStrikeShader{ nullptr };
        Shader* mShadowMappingShader{ nullptr };
        Shader* mBasicShader{ nullptr };
        Shader* mCrossSection{ nullptr };
    };
}