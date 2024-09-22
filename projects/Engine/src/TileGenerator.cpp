#include "TileGenerator.h"

Canavar::Engine::TileGenerator::TileGenerator(int resolution, int tiles, float width)
    : QObject()
    , mResolution(resolution)
    , mTiles(tiles)
    , mWidth(width)
{
    const int n = mResolution;

    // Vertices
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            float x = j * mWidth / (n - 1) - mWidth / 2.0f;
            float y = 0;
            float z = i * mWidth / (n - 1) - mWidth / 2.0f;
            float u = float(j) / (n - 1);
            float v = float(n - i - 1) / (n - 1);

            Vertex vertex;
            vertex.position = QVector3D(x, y, z);
            vertex.normal = QVector3D(0, 1, 0);
            vertex.texture = QVector2D(u, v);

            mVertices << vertex;
        }
    }

    // Indices
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - 1; j++)
        {
            mIndices << n * i + j;
            mIndices << n * (i + 1) + j;
            mIndices << n * i + j + 1;

            mIndices << n * (i + 1) + j;
            mIndices << n * (i + 1) + j + 1;
            mIndices << n * i + j + 1;
        }
    }

    // Tiles
    const QVector2D I = QVector2D(1, 0) * mWidth;
    const QVector2D J = QVector2D(0, 1) * mWidth;

    mTilePositions.resize(mTiles * mTiles);

    for (int i = 0; i < mTiles; i++)
        for (int j = 0; j < mTiles; j++)
            mTilePositions[j + i * mTiles] = (float)(j - mTiles / 2) * I + (float)(i - mTiles / 2) * J;

    // OpenGL Stuff
    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mTilePositions.size() * sizeof(QVector2D), mTilePositions.constData(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.constData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.constData(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

Canavar::Engine::TileGenerator::~TileGenerator()
{
    // TODO
}

QVector2D Canavar::Engine::TileGenerator::WhichTile(const QVector3D& subject) const
{
    int i = int(subject.x()) / mWidth;
    int j = int(subject.z()) / mWidth;

    return QVector2D(i * mWidth, j * mWidth);
}

void Canavar::Engine::TileGenerator::TranslateTiles(const QVector2D& translation)
{
    for (int i = 0; i < mTilePositions.size(); ++i)
        mTilePositions[i] += translation;

    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mTilePositions.size() * sizeof(QVector2D), mTilePositions.constData());
}

void Canavar::Engine::TileGenerator::Render(GLenum primitive)
{
    glBindVertexArray(mVAO);
    glDrawElementsInstanced(primitive, (mResolution - 1) * (mResolution - 1) * 2 * 3, GL_UNSIGNED_INT, 0, mTilePositions.size());
    glBindVertexArray(0);
}