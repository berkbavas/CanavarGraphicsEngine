#include "CubeStrip.h"

Canavar::Engine::CubeStrip::CubeStrip()
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_STRIP), CUBE_STRIP, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Canavar::Engine::CubeStrip::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_LINE_STRIP, 0, 17);
    glBindVertexArray(0);
}
