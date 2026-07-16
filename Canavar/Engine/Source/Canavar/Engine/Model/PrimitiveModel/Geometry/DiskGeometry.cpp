#include "DiskGeometry.h"

#include <cmath>
#include <vector>

Canavar::Engine::DiskGeometry::DiskGeometry(int Segments)
    : mSegments(Segments)
{}

void Canavar::Engine::DiskGeometry::Initialize()
{
    initializeOpenGLFunctions();

    // Layout: position(3) + normal(3) = 6 floats per vertex
    // Vertex 0: center
    // Vertices 1..N: edge points
    struct PosNorm
    {
        float x, y, z;   // position
        float nx, ny, nz; // normal (0,1,0) for all disk vertices
    };

    std::vector<PosNorm> Vertices;
    Vertices.reserve(static_cast<size_t>(mSegments + 1));

    // Center
    Vertices.push_back({ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f });

    const float Step = 2.0f * static_cast<float>(M_PI) / static_cast<float>(mSegments);
    for (int i = 0; i < mSegments; ++i)
    {
        const float Angle = static_cast<float>(i) * Step;
        Vertices.push_back({ std::cos(Angle), 0.0f, std::sin(Angle), 0.0f, 1.0f, 0.0f });
    }

    // Indices: fan triangles (0, i, i+1) wrapping around
    std::vector<unsigned int> Indices;
    Indices.reserve(static_cast<size_t>(mSegments) * 3);
    for (int i = 1; i <= mSegments; ++i)
    {
        Indices.push_back(0);
        Indices.push_back(static_cast<unsigned int>(i));
        Indices.push_back(static_cast<unsigned int>(i % mSegments + 1));
    }

    mMode   = GL_TRIANGLES;
    mCount  = static_cast<GLsizei>(Indices.size());
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
