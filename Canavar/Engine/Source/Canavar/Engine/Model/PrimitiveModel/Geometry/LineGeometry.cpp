#include "LineGeometry.h"

Canavar::Engine::LineGeometry::LineGeometry()
{
    initializeOpenGLFunctions();

    // Two vertices: start and end of the unit line segment along +Z
    const float Vertices[] = {
        0.0f, 0.0f, 0.0f, // start
        0.0f, 0.0f, 1.0f, // end
    };

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Canavar::Engine::LineGeometry::~LineGeometry()
{
    if (mVAO)
    {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
    }

    if (mVBO)
    {
        glDeleteBuffers(1, &mVBO);
        mVBO = 0;
    }
}

void Canavar::Engine::LineGeometry::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}
