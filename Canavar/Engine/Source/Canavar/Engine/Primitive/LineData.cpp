#include "LineData.h"

Canavar::Engine::LineData::LineData()
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DUMMY_POINT), &DUMMY_POINT, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Canavar::Engine::LineData::~LineData()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void Canavar::Engine::LineData::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
}
