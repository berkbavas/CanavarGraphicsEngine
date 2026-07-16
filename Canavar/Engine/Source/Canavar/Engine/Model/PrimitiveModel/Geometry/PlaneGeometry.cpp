#include "PlaneGeometry.h"

#include "Canavar/Engine/Util/Logger.h"

#include <QVector3D>

Canavar::Engine::PlaneGeometry::PlaneGeometry()
{
    LOG_DEBUG("PlaneGeometry::PlaneGeometry: Initializing PlaneGeometry...");

    struct Vertex
    {
        QVector3D Position;
        QVector3D Normal;
    };

    //               +y
    //                |
    //      (-1,1,0) |   (1,1,0)
    //        B *-----|------* A
    //          |     |      |
    //  -x <----|-----*------|----> +x
    //          |     |      |
    //        C *-----|------* D
    //      (-1,-1,0)  |   (1,-1,0)
    //                |
    //               -y

    constexpr QVector3D A = QVector3D(1, 1, 0);
    constexpr QVector3D B = QVector3D(-1, 1, 0);
    constexpr QVector3D C = QVector3D(-1, -1, 0);
    constexpr QVector3D D = QVector3D(1, -1, 0);
    constexpr QVector3D NORMAL = QVector3D(0, 0, -1);

    constexpr Vertex VERTICES[6] = { { A, NORMAL },   //
                                     { B, NORMAL },   //
                                     { C, NORMAL },   //
                                     { C, NORMAL },   //
                                     { D, NORMAL },   //
                                     { A, NORMAL } }; //
    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    LOG_DEBUG("PlaneGeometry::PlaneGeometry: PlaneGeometry has been initialized.");
}

Canavar::Engine::PlaneGeometry::~PlaneGeometry()
{
    LOG_DEBUG("PlaneGeometry::~PlaneGeometry: Deleting OpenGL stuff...");

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

    LOG_DEBUG("PlaneGeometry::~PlaneGeometry: OpenGL stuff has been deleted.");
}

void Canavar::Engine::PlaneGeometry::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
