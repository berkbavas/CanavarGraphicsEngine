#include "PlaneData.h"

#include <QVector3D>

Canavar::Engine::PlaneData::PlaneData()
{
    struct Vertex
    {
        QVector3D Position;
        QVector3D Normal;
    };

    constexpr Vertex VERTICES[6] = {
        { QVector3D(-1.0f, 0.0f, -1.0f), QVector3D(0.0f, 1.0f, 0.0f) }, //
        { QVector3D(1.0f, 0.0f, -1.0f), QVector3D(0.0f, 1.0f, 0.0f) },  //
        { QVector3D(1.0f, 0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f) },   //

        { QVector3D(1.0f, 0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f) },   //
        { QVector3D(-1.0f, 0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f) },  //
        { QVector3D(-1.0f, 0.0f, -1.0f), QVector3D(0.0f, 1.0f, 0.0f) }, //
    };

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Normal)));

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
