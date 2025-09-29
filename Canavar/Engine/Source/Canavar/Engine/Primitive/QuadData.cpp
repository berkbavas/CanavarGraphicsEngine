#include "QuadData.h"

#include <QVector2D>

Canavar::Engine::QuadData::QuadData()
{
    struct Vertex
    {
        QVector2D Position;
        QVector2D TexCoord;
    };

    static constexpr Vertex VERTICES[6] = {
        // First Triangle
        { QVector2D(-1.0f, +1.0f), QVector2D(0.0f, 1.0f) }, // Top Left
        { QVector2D(-1.0f, -1.0f), QVector2D(0.0f, 0.0f) }, // Bottom Left
        { QVector2D(+1.0f, -1.0f), QVector2D(1.0f, 0.0f) }, // Bottom Right

        // Second Triangle
        { QVector2D(-1.0f, +1.0f), QVector2D(0.0f, 1.0f) }, // Top Left
        { QVector2D(+1.0f, -1.0f), QVector2D(1.0f, 0.0f) }, // Bottom Right
        { QVector2D(+1.0f, +1.0f), QVector2D(1.0f, 1.0f) }  // Top Right
    };

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoord));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Canavar::Engine::QuadData::~QuadData()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void Canavar::Engine::QuadData::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}