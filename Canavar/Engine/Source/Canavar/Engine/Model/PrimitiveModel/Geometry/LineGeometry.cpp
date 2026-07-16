#include "LineGeometry.h"

void Canavar::Engine::LineGeometry::Initialize()
{
    initializeOpenGLFunctions();

    // Two vertices: start and end of the unit line segment along +Z
    const float Vertices[] = {
        0.0f, 0.0f, 0.0f, // start
        0.0f, 0.0f, 1.0f, // end
    };

    mMode   = GL_LINES;
    mCount  = 2;
    mUseEBO = false;

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
