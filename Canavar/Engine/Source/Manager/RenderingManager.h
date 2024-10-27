#pragma once

#include "Core/Constants.h"
#include "Manager/Manager.h"
#include "Node/Camera/Camera.h"
#include "Node/Light/DirectionalLight.h"
#include "Node/Model/Model.h"
#include "Node/Sky/Sky.h"
#include "Node/Terrain/Terrain.h"
#include "Util/Shader.h"

#include <map>
#include <memory>

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

namespace Canavar::Engine
{
    class ShaderManager;
    class NodeManager;
    class CameraManager;
    class LightManager;

    enum E_Framebuffer
    {
        Default,
        Temp
    };

    class RenderingManager : public Manager, protected QOpenGLExtraFunctions
    {
      public:
        explicit RenderingManager(QObject *parent = nullptr);

        void Initialize() override;
        void PostInitialize() override;

        void Render(float ifps);
        void Resize(int width, int height);

      private:
        void RenderModel(ModelPtr pModel);

        void SetUniforms();
        void SetCommonUniforms(Shader *pShader);
        void SetDirectionalLights(Shader *pShader);
        void SetPointLights(Shader *pShader, NodePtr pNode);

        void ResizeFramebuffers();

        ShaderManager *mShaderManager;
        NodeManager *mNodeManager;
        CameraManager *mCameraManager;
        LightManager *mLightManager;

        CameraPtr mActiveCamera{ nullptr };

        Shader *mModelShader{ nullptr };
        Shader *mSkyShader{ nullptr };
        Shader *mTerrainShader{ nullptr };

        SkyPtr mSky;
        TerrainPtr mTerrain;
        DirectionalLightPtr mSun;

        std::map<E_Framebuffer, QOpenGLFramebufferObject *> mFramebuffers;
        std::map<E_Framebuffer, QOpenGLFramebufferObjectFormat> mFramebufferFormats;

        int mWidth{ INITIAL_WIDTH };
        int mHeight{ INITIAL_HEIGHT };
    };
};
