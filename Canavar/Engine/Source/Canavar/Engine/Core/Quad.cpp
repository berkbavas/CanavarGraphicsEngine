#include "Quad.h"

#include <QVector2D>

Canavar::Engine::Quad::Quad()
{
    struct Vertex
    {
        QVector2D Position;
        QVector2D TexCoord;
    };

    // Vertex data for a full-screen quad (two triangles) with positions and texture coordinates
    const Vertex Vertices[6] = {
        { QVector2D(-1.0f, -1.0f), QVector2D(0.0f, 0.0f) }, // Bottom-left
        { QVector2D(1.0f, -1.0f), QVector2D(1.0f, 0.0f) },  // Bottom-right
        { QVector2D(-1.0f, 1.0f), QVector2D(0.0f, 1.0f) },  // Top-left
        { QVector2D(1.0f, -1.0f), QVector2D(1.0f, 0.0f) },  // Bottom-right
        { QVector2D(1.0f, 1.0f), QVector2D(1.0f, 1.0f) },   // Top-right
        { QVector2D(-1.0f, 1.0f), QVector2D(0.0f, 1.0f) }   // Top-left
    };

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoord));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Canavar::Engine::Quad::~Quad()
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

void Canavar::Engine::Quad::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
