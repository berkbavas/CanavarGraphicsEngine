#include "Plane.h"

Canavar::Engine::Plane::Plane()
{
    //               +y
    //                |
    //      (-1,1,0)  |   (1,1,0)
    //          *-----|------*
    //          |     |      |
    //  -x <----|-----*------|----> +x
    //          |     |      |
    //          *-----|------*
    //      (-1,-1,0) |   (1,-1,0)
    //                |
    //               -y

    constexpr QVector4D VERTICES[6] = { QVector4D(-1, -1, 0, 1),  QVector4D(1, -1, 0, 1), QVector4D(-1, 1, 0, 1), //
                                        QVector4D(1, 1, 0, 1), QVector4D(-1, 1, 0, 1), QVector4D(1, -1, 0, 1) };

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(QVector4D), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Canavar::Engine::Plane::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
