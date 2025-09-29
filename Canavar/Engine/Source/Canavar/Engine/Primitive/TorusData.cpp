#include "TorusData.h"

#include <QVector3D>
#include <QVector>

Canavar::Engine::TorusData::TorusData(float R, float r, int SegU, int SegV)
{
    // Initialize the torus geometry
    struct Vertex
    {
        QVector3D Position;
        QVector3D Normal;
    };

    QVector<Vertex> Vertices;
    QVector<uint32_t> Indices;

    SegU = std::max(3, SegU);
    SegV = std::max(3, SegV);
    Vertices.reserve((SegU + 1) * (SegV + 1));
    Indices.reserve(SegU * SegV * 6);

    for (int u = 0; u <= SegU; ++u)
    {
        float tu = float(u) / SegU;
        float au = tu * 2 * M_PI;
        float cu = std::cos(au);
        float su = std::sin(au);

        QVector3D C = QVector3D(R * cu, R * su, 0.0f); // ring center

        // local frame for tube
        QVector3D T = QVector3D(-su, cu, 0.0f).normalized();           // tangent along ring
        QVector3D B = QVector3D(0, 0, 1);                              // binormal (constant for torus in XY)
        QVector3D N_ring = QVector3D::crossProduct(B, T).normalized(); // outward from ring center

        for (int v = 0; v <= SegV; ++v)
        {
            float tv = float(v) / SegV;
            float av = tv * 2 * M_PI;
            float cv = std::cos(av), sv = std::sin(av);

            // circle around (N_ring, B) plane
            QVector3D nrm = (cv * N_ring + sv * B).normalized();
            QVector3D pos = C + r * nrm;

            Vertices.push_back({ pos, nrm });
        }
    }

    auto IndexAt = [&](int u, int v) { return u * (SegV + 1) + v; };

    for (int u = 0; u < SegU; ++u)
    {
        for (int v = 0; v < SegV; ++v)
        {
            uint32_t a = IndexAt(u, v);
            uint32_t b = IndexAt(u + 1, v);
            uint32_t c = IndexAt(u + 1, v + 1);
            uint32_t d = IndexAt(u, v + 1);
            Indices.push_back(a);
            Indices.push_back(b);
            Indices.push_back(c);
            Indices.push_back(a);
            Indices.push_back(c);
            Indices.push_back(d);
        }
    }

    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32_t), Indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    mNumberOfIndices = static_cast<int>(Indices.size());
}

Canavar::Engine::TorusData::~TorusData()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}

void Canavar::Engine::TorusData::Render()
{
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mNumberOfIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
