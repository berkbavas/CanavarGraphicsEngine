#pragma once

#include "Canavar/Engine/Camera/FreeCamera.h"
#include "Canavar/Engine/Core/CubicFramebuffer.h"
#include "Canavar/Engine/Core/Quad.h"
#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Manager/Manager.h"

#include <memory>
#include <vector>

#include <QOpenGLFunctions_4_5_Core>
#include <QVector3D>

namespace Canavar::Engine
{
    class Renderer;
    class Terrain;
    class Sphere;

    class LineOfSightAnalyzer : public Manager, protected QOpenGLFunctions_4_5_Core
    {
      public:
        explicit LineOfSightAnalyzer(Renderer *pRenderer);

        void Initialize() override;
        void Update(float Ifps) override;
        void SetTerrain(Terrain *pTerrain);

        float GetVisibilityOpacity() const { return mVisibilityOpacity; }
        float GetMinLosDistance() const { return mMinLosDistance; }
        float GetMaxLosDistance() const { return mMaxLosDistance; }
        const QVector3D &GetObserverPosition() const { return mObserverPosition; }
        float GetBias() const { return mBias; }
        float GetFarPlane() const;
        bool IsEnabled() const { return mEnabled; }
        GLuint GetDepthMap() const;
        float GetShadowOpacity() const { return mShadowOpacity; }
        const QVector3D &GetShadowColor() const { return mShadowColor; }
        float GetObserverHeight() const { return mObserverHeight; }
        GLuint GetDebugTexture() const { return mDebugTexture; }
        int GetDebugTextureSize() const { return mDebugTextureSize; }

        void SetMinLosDistance(float Distance);
        void SetMaxLosDistance(float Distance);
        void SetObserverPosition(const QVector3D &Position);
        void SetBias(float Bias);
        void SetEnabled(bool Enabled);
        void SetVisibilityOpacity(float Opacity);
        void SetShadowOpacity(float Opacity);
        void SetShadowColor(const QVector3D &Color);
        void SetObserverHeight(float Height);
        void RenderDebugFace(int FaceIndex);

      private:
        void UpdateObserverCameras();
        void CreateObserverCameras();
        void RenderDebugFaceInner(int FaceIndex);
        bool ShouldRender() const;

        Renderer *mRenderer{ nullptr };

        CubicFramebufferPtr mFramebuffer{ nullptr };
        Terrain *mTerrain{ nullptr };
        ShaderPtr mShader{ nullptr };

        QuadPtr mDebugQuad{ nullptr };
        ShaderPtr mDebugShader{ nullptr };
        GLuint mDebugFBO{ 0 };
        GLuint mDebugTexture{ 0 };
        static constexpr int mDebugTextureSize{ 512 };
        bool mRenderDebug{ false };
        int mLosDebugFaceIndex{ 0 };

        float mMinLosDistance{ 10.0f };
        float mMaxLosDistance{ 500.0f };
        QVector3D mObserverPosition{ 0, 0, 0 };
        float mObserverHeight{ 10.0f };
        float mBias{ 0.01f };
        float mVisibilityOpacity{ 0.35f };
        float mShadowOpacity{ 0.3f };
        QVector3D mShadowColor{ 0.0f, 0.0f, 0.0f };

        std::vector<FreeCameraPtr> mObserverCameras;

        int mWidth{ 4096 };
        int mHeight{ 4096 };
        bool mEnabled{ false };
        bool mIsDirty{ true };

        Sphere *mObserverSphere{ nullptr };
    };

    using LineOfSightAnalyzerPtr = std::unique_ptr<LineOfSightAnalyzer>;
}