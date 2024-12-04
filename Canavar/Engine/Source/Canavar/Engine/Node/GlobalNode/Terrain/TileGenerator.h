#pragma once

#include <QOpenGLExtraFunctions>
#include <QVector2D>
#include <QVector3D>

namespace Canavar::Engine
{
    class TileGenerator : protected QOpenGLExtraFunctions
    {
      public:
        struct Vertex
        {
            QVector3D position;
            QVector3D normal;
            QVector2D texture;
        };

        TileGenerator(int resolution, int tiles, float width);

        void Render(GLenum primitive);

        QVector2D WhichTile(const QVector3D& subject) const;
        void TranslateTiles(const QVector2D& translation);

      private:
        QVector<QVector2D> mTilePositions;
        QVector<Vertex> mVertices;
        QVector<unsigned int> mIndices;

        int mResolution;
        int mTiles;
        float mWidth;

        // OpenGL Stuff
        unsigned int mVAO;
        unsigned int mVBO;
        unsigned int mEBO;
        unsigned int mPBO;
    };
}
