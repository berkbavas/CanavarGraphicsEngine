#include "Quad.h"

Canavar::Engine::Quad::Quad()
{
    constexpr float VERTICES[24] = { -1.0f, +1.0f, 0.0f, 1.0f, //
                                     -1.0f, -1.0f, 0.0f, 0.0f, //
                                     +1.0f, -1.0f, 1.0f, 0.0f, //
                                     -1.0f, +1.0f, 0.0f, 1.0f, //
                                     +1.0f, -1.0f, 1.0f, 0.0f, //
                                     +1.0f, +1.0f, 1.0f, 1.0f };
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Canavar::Engine::Quad::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
