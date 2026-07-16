#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/GlobalNode/GlobalNode.h"
#include "Canavar/Engine/Util/Macros.h"

#include <QImage>
#include <QOpenGLFunctions_4_5_Core>
#include <QVector3D>
#include <QVector>

namespace Canavar::Engine
{
    class Renderer;
    class PerspectiveCamera;

    class Terrain : public GlobalNode, protected QOpenGLFunctions_4_5_Core
    {
      public:
        Terrain(Renderer* pRenderer);
        ~Terrain();

        const char* GetNodeTypeName() const override { return "Terrain"; }
        void Render();

      private:
        void Generate();
        void SetUpBuffers();
        void SetUpTextures();
        void SetUpShader();
        GLuint Load2DArray(const std::array<std::string, 4>& Filepaths, int Miplevels);
        QVector2D WhichTile(const QVector3D& Subject) const;
        void TranslateTiles(const QVector2D& Translation);
        void CalculateTilePositions(PerspectiveCamera* pCamera);

        Renderer* mRenderer{ nullptr };
        ShaderPtr mTerrainShader{ nullptr };

        QVector<QVector2D> mTilePositions;
        QVector<QVector3D> mPositions;
        QVector<unsigned int> mIndices;

        QVector2D mPreviousTilePosition;

        int mSubdivision{ 4 };
        int mTiles{ 32 };
        float mWidth{ 2048.0f };

        GLuint mVAO{ 0 };
        GLuint mEBO{ 0 };
        GLuint mVBO{ 0 };
        GLuint mPBO{ 0 };

        DEFINE_MEMBER(int, Octaves, 7);
        DEFINE_MEMBER(float, Amplitude, 1055.0f);
        DEFINE_MEMBER(float, Frequency, 0.110f);
        DEFINE_MEMBER(float, Persistence, 0.063f);
        DEFINE_MEMBER(float, Lacunarity, 8.150f);
        DEFINE_MEMBER(float, TessellationMultiplier, 8.0f);

        DEFINE_MEMBER(float, Ambient, 0.25f);
        DEFINE_MEMBER(float, Diffuse, 0.75f);
        DEFINE_MEMBER(float, Specular, 0.25f);
        DEFINE_MEMBER(float, Shininess, 8.0f);
        DEFINE_MEMBER(bool, Enabled, false);

        GLuint mAlbedoTexture{ 0 };
        GLuint mNormalTexture{ 0 };
        GLuint mDisplacementTexture{ 0 };

        std::array<float, 4> mTextureStartHeights{ 0, 270, 520, 866 };
        std::array<float, 4> mTextureBlends{ 0.0, 80, 415, 610 };
        std::array<float, 4> mTextureSizes{ 70.40F, 16.229F, 6.629F, 11.657F };
        std::array<float, 4> mTextureDisplacementWeights{ 0.5, 0.671, 0.676, 0.869 };
    };

    using TerrainPtr = std::unique_ptr<Terrain>;
}