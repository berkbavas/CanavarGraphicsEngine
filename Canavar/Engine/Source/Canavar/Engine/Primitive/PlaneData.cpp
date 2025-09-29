#include "PlaneData.h"

#include <QVector3D>

Canavar::Engine::PlaneData::PlaneData()
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

    struct Vertex
    {
        QVector3D Position;
        QVector3D Normal;
    };

    constexpr Vertex VERTICES[6] = {
        // First Triangle
        { QVector3D(-1.0f, +1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f) }, // Top Left
        { QVector3D(-1.0f, -1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f) }, // Bottom Left
        { QVector3D(+1.0f, -1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f) }, // Bottom Right

        // Second Triangle
        { QVector3D(-1.0f, +1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f) }, // Top Left
        { QVector3D(+1.0f, -1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f) }, // Bottom Right
        { QVector3D(+1.0f, +1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f) }  // Top Right
    };

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

Canavar::Engine::PlaneData::~PlaneData()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void Canavar::Engine::PlaneData::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
