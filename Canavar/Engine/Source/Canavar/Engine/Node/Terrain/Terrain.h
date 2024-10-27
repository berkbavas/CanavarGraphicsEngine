#pragma once

#include "Canavar/Engine/Node/Node.h"
#include "Canavar/Engine/Node/Terrain/TileGenerator.h"
#include "Canavar/Engine/Util/Macros.h"

#include <memory>

#include <QObject>
#include <QOpenGLExtraFunctionsPrivate>
#include <QOpenGLTexture>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

namespace Canavar::Engine
{
    class Shader;
    class DirectionalLight;
    class Camera;

    class Terrain : public Node, protected QOpenGLExtraFunctions
    {
      public:
        void Initialize();
        void Render(Shader* pShader, Camera* pCamera);
        void Reset();

      private:
        TileGenerator* mTileGenerator;

        QMap<QString, QOpenGLTexture*> mTextures;

        QVector2D mPreviousTilePosition;

        DEFINE_MEMBER(float, Amplitude);
        DEFINE_MEMBER(float, Frequency);
        DEFINE_MEMBER(int, Octaves);
        DEFINE_MEMBER(float, Power);
        DEFINE_MEMBER(QVector3D, Seed);
        DEFINE_MEMBER(float, TessellationMultiplier);

        DEFINE_MEMBER(float, GrassCoverage);
        DEFINE_MEMBER(float, Ambient);
        DEFINE_MEMBER(float, Diffuse);
        DEFINE_MEMBER(float, Specular);
        DEFINE_MEMBER(float, Shininess);

        DEFINE_MEMBER(bool, Enabled, true);

        friend class NodeManager;
    };

    using TerrainPtr = std::shared_ptr<Terrain>;
}
