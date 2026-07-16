#include "PlaneGeometry.h"

void Canavar::Engine::PlaneGeometry::Initialize()
{
    initializeOpenGLFunctions();

    // position(3) + normal(3) per vertex
    const float Vertices[] = {
        -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, //
        0.5f,  0.0f, -0.5f, 0.0f, 1.0f, 0.0f, //
        0.5f,  0.0f, 0.5f,  0.0f, 1.0f, 0.0f, //
        -0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.0f, //
    };

    const unsigned int Indices[] = {
        0, 1, 2, 0, 2, 3,
    };

    mMode = GL_TRIANGLES;
    mCount = 6;
    mUseEBO = true;

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
