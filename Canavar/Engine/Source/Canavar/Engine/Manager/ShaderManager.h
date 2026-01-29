#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/Manager.h"

#include <map>

namespace Canavar::Engine
{
    class ShaderManager : public Manager
    {
      public:
        using Manager::Manager;

        void Initialize() override;
        void Shutdown() override;

        Shader* GetShader(ShaderType ShaderType);

      private:
        std::map<ShaderType, Shader*> mShaders;

        Shader* mModelShader{ nullptr };
        Shader* mSkyShader{ nullptr };
        Shader* mNozzleEffect{ nullptr };
        Shader* mLineShader{ nullptr };
        Shader* mLightningStrikeShader{ nullptr };
        Shader* mShadowMappingShader{ nullptr };
        Shader* mBasicShader{ nullptr };
        Shader* mCrossSection{ nullptr };
        Shader* mTerrainShader{ nullptr };
        Shader* mText2DShader{ nullptr };
        Shader* mText3DShader{ nullptr };
        Shader* mCinematicShader{ nullptr };
        Shader* mAcesShader{ nullptr };
        Shader* mScreenShader{ nullptr };
        Shader* mBloomShader{ nullptr };
        Shader* mBlurShader{ nullptr };
        Shader* mFXAAShader{ nullptr };
        Shader* mColorGradingShader{ nullptr };
    };
}