#include "CircleGeometry.h"

#include <cmath>
#include <vector>

Canavar::Engine::CircleGeometry::CircleGeometry(int Segments)
    : mSegments(Segments)
{}

void Canavar::Engine::CircleGeometry::Initialize()
{
    initializeOpenGLFunctions();

    // N+1 vertices: last vertex equals the first to close the loop
    std::vector<float> Vertices;
    Vertices.reserve(static_cast<size_t>(mSegments + 1) * 3);

    const float Step = 2.0f * static_cast<float>(M_PI) / static_cast<float>(mSegments);

    for (int i = 0; i <= mSegments; ++i)
    {
        const float Angle = static_cast<float>(i) * Step;
        Vertices.push_back(std::cos(Angle)); // X
        Vertices.push_back(0.0f);            // Y
        Vertices.push_back(std::sin(Angle)); // Z
    }

    mMode = GL_LINE_STRIP;
    mCount = static_cast<GLsizei>(mSegments + 1);
    mUseEBO = false;

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(Vertices.size() * sizeof(float)), Vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
