#include "SphereGeometry.h"

#include <cmath>
#include <vector>

Canavar::Engine::SphereGeometry::SphereGeometry(int Stacks, int Sectors)
    : mStacks(Stacks)
    , mSectors(Sectors)
{}

void Canavar::Engine::SphereGeometry::Initialize()
{
    initializeOpenGLFunctions();

    struct PosNorm
    {
        float x, y, z;
        float nx, ny, nz;
    };

    std::vector<PosNorm> Vertices;
    Vertices.reserve(static_cast<size_t>((mStacks + 1) * (mSectors + 1)));

    const float StackStep = static_cast<float>(M_PI) / static_cast<float>(mStacks);
    const float SectorStep = 2.0f * static_cast<float>(M_PI) / static_cast<float>(mSectors);

    for (int i = 0; i <= mStacks; ++i)
    {
        const float StackAngle = static_cast<float>(M_PI) / 2.0f - static_cast<float>(i) * StackStep;
        const float Xy = std::cos(StackAngle);
        const float Z = std::sin(StackAngle);

        for (int j = 0; j <= mSectors; ++j)
        {
            const float SectorAngle = static_cast<float>(j) * SectorStep;
            const float X = Xy * std::cos(SectorAngle);
            const float Y = Xy * std::sin(SectorAngle);

            // On a unit sphere the position equals the normal
            Vertices.push_back({ X, Z, Y, X, Z, Y });
        }
    }

    std::vector<unsigned int> Indices;
    Indices.reserve(static_cast<size_t>(mStacks * mSectors * 6));

    for (int i = 0; i < mStacks; ++i)
    {
        for (int j = 0; j < mSectors; ++j)
        {
            const unsigned int k1 = static_cast<unsigned int>(i * (mSectors + 1) + j);
            const unsigned int k2 = k1 + static_cast<unsigned int>(mSectors + 1);

            // First triangle
            Indices.push_back(k1);
            Indices.push_back(k2);
            Indices.push_back(k1 + 1);

            // Second triangle
            Indices.push_back(k1 + 1);
            Indices.push_back(k2);
            Indices.push_back(k2 + 1);
        }
    }

    mMode = GL_TRIANGLES;
    mCount = static_cast<GLsizei>(Indices.size());
    mUseEBO = true;

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(Indices.size() * sizeof(unsigned int)), Indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(Vertices.size() * sizeof(PosNorm)), Vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PosNorm), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PosNorm), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
