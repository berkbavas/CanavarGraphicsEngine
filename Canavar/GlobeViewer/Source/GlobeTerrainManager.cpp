#include "GlobeTerrainManager.h"

#include <QOpenGLTexture>
#include <QFile>

#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Canavar::Globe
{
    GlobeTerrainManager::GlobeTerrainManager(const QString& TerrainRoot)
        : mTerrainRoot(TerrainRoot)
        , mLoader(TerrainRoot)
    {}

    GlobeTerrainManager::~GlobeTerrainManager()
    {
        for (auto& [Id, Tile] : mTileCache)
            ReleaseTile(*Tile);
    }

    bool GlobeTerrainManager::Initialize()
    {
        if (!initializeOpenGLFunctions()) return false;

        // Load quadtree index
        const QString IndexPath = mTerrainRoot + "/index.tidx";
        if (!mIndex.Load(IndexPath)) return false;

        // Compile shader
        mShader = std::make_unique<QOpenGLShaderProgram>();

        const QString VertPath = "Resources/Shaders/GlobeTerrain.vert";
        const QString FragPath = "Resources/Shaders/GlobeTerrain.frag";

        if (!mShader->addShaderFromSourceFile(QOpenGLShader::Vertex,   VertPath) ||
            !mShader->addShaderFromSourceFile(QOpenGLShader::Fragment,  FragPath) ||
            !mShader->link())
        {
            return false;
        }

        mInitialized = true;
        return true;
    }

    // ─────────────────────────────────────────────────────────────────────────

    float GlobeTerrainManager::ScreenSpaceError(const TerrainIndexNode& Node,
                                                  const GlobeCamera& Camera,
                                                  float ViewportH, float FovRad) const
    {
        // Distance from camera ECEF origin to tile centre
        const auto& EC = Camera.EcefOrigin();
        const double Dx = Node.CenterEcefX - EC[0];
        const double Dy = Node.CenterEcefY - EC[1];
        const double Dz = Node.CenterEcefZ - EC[2];
        const double Dist = std::sqrt(Dx*Dx + Dy*Dy + Dz*Dz);

        if (Dist < 1.0) return 1e9f; // camera inside tile

        // λ = ViewportH / (2 * tan(fov/2)) = focal length in pixels
        const float Lambda = ViewportH / (2.0f * std::tan(FovRad * 0.5f));
        return Node.GeometricError * Lambda / static_cast<float>(Dist);
    }

    void GlobeTerrainManager::TraverseNode(int NodeIdx,
                                            const GlobeCamera& Camera,
                                            float ViewportH, float FovRad)
    {
        if (NodeIdx < 0 || NodeIdx >= mIndex.NodeCount()) return;

        const TerrainIndexNode& Node = mIndex.GetNode(NodeIdx);
        const TileId Id = { Node.TileLevel, Node.TileX, Node.TileY };

        const float SSE = ScreenSpaceError(Node, Camera, ViewportH, FovRad);
        const bool  Refine = (SSE > mPixelErrorThreshold) && Node.ChildMask;

        if (Refine)
        {
            // Recurse into children
            for (int Q = 0; Q < 4; ++Q)
            {
                if ((Node.ChildMask >> Q) & 1)
                    TraverseNode(Node.ChildIndices[Q], Camera, ViewportH, FovRad);
            }
        }
        else
        {
            // Render this tile if it has data
            if (!Node.HasData) return;

            const auto It = mTileCache.find(Id);
            if (It != mTileCache.end() && It->second->IsReady())
            {
                mRenderList.push_back(It->second.get());
            }
            else if (!mLoader.IsPending(Id))
            {
                mLoader.RequestLoad(Id);
            }
        }
    }

    void GlobeTerrainManager::SelectTiles(const GlobeCamera& Camera,
                                           float ViewportH, float FovRad)
    {
        mRenderList.clear();
        for (int RootIdx : mIndex.RootNodeIndices())
            TraverseNode(RootIdx, Camera, ViewportH, FovRad);
    }

    // ─────────────────────────────────────────────────────────────────────────

    void GlobeTerrainManager::UploadResult(std::unique_ptr<TileLoadResult> Result)
    {
        if (!Result->Success) return;

        auto Tile = std::make_unique<GlobeTerrainTile>();
        Tile->Id           = Result->Id;
        Tile->Bounds       = Result->Bounds;
        Tile->CenterEcef   = Result->CenterEcef;
        Tile->GeometricError = Result->GeometricError;
        Tile->IndexCount   = static_cast<uint32_t>(Result->Indices.size());

        // ── VAO ──────────────────────────────────────────────────────────────
        glGenVertexArrays(1, &Tile->Vao);
        glBindVertexArray(Tile->Vao);

        glGenBuffers(1, &Tile->Vbo);
        glBindBuffer(GL_ARRAY_BUFFER, Tile->Vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(Result->Vertices.size() * sizeof(TerrainVertex)),
                     Result->Vertices.constData(), GL_STATIC_DRAW);

        // layout(location=0) aPosition
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
                              reinterpret_cast<void*>(offsetof(TerrainVertex, Px)));
        glEnableVertexAttribArray(0);

        // layout(location=1) aNormal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
                              reinterpret_cast<void*>(offsetof(TerrainVertex, Nx)));
        glEnableVertexAttribArray(1);

        // layout(location=2) aTexCoord
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex),
                              reinterpret_cast<void*>(offsetof(TerrainVertex, U)));
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &Tile->Ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Tile->Ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(Result->Indices.size() * sizeof(uint32_t)),
                     Result->Indices.constData(), GL_STATIC_DRAW);

        glBindVertexArray(0);

        // ── Texture ──────────────────────────────────────────────────────────
        const QImage TexData = Result->Texture.convertToFormat(QImage::Format_RGBA8888)
                                              .mirrored(false, true); // flip for GL

        glGenTextures(1, &Tile->Texture);
        glBindTexture(GL_TEXTURE_2D, Tile->Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     TexData.width(), TexData.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, TexData.constBits());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        mTileCache[Result->Id] = std::move(Tile);
    }

    void GlobeTerrainManager::ReleaseTile(GlobeTerrainTile& Tile)
    {
        if (Tile.Vao)     glDeleteVertexArrays(1, &Tile.Vao);
        if (Tile.Vbo)     glDeleteBuffers(1, &Tile.Vbo);
        if (Tile.Ibo)     glDeleteBuffers(1, &Tile.Ibo);
        if (Tile.Texture) glDeleteTextures(1, &Tile.Texture);
        Tile.Vao = Tile.Vbo = Tile.Ibo = Tile.Texture = 0;
    }

    // ─────────────────────────────────────────────────────────────────────────

    void GlobeTerrainManager::Render(const GlobeCamera& Camera,
                                      int ViewportW, int ViewportH)
    {
        if (!mInitialized) return;

        // Upload any completed tile loads
        while (auto Result = mLoader.PollResult())
            UploadResult(std::move(Result));

        // Select which tiles to draw this frame
        const float FovRad = Camera.ProjectionMatrix().column(1).y()
                             > 0 ? 2.0f * std::atan(1.0f / Camera.ProjectionMatrix().column(1).y())
                                 : static_cast<float>(M_PI / 3.0);
        SelectTiles(Camera, static_cast<float>(ViewportH), FovRad);

        if (mRenderList.empty()) return;

        // ── Setup render state ────────────────────────────────────────────────
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        if (mWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        mShader->bind();

        // Camera ECEF origin
        const auto EC = Camera.EcefOrigin();

        // Uniforms constant across tiles
        mShader->setUniformValue("uProjection", Camera.ProjectionMatrix());
        mShader->setUniformValue("uView",       Camera.ViewMatrix());
        mShader->setUniformValue("uAlbedo",      0);
        mShader->setUniformValue("uWireframe",   mWireframe ? 1.0f : 0.0f);

        // Simple sun direction (towards the sun at ~45° above horizon)
        const auto SunENU = ComputeENUBasis(Camera.Latitude(), Camera.Longitude());
        const QVector3D SunDir(
            static_cast<float>(SunENU.North[0] * 0.7 + SunENU.Up[0] * 0.7),
            static_cast<float>(SunENU.North[1] * 0.7 + SunENU.Up[1] * 0.7),
            static_cast<float>(SunENU.North[2] * 0.7 + SunENU.Up[2] * 0.7)
        );
        mShader->setUniformValue("uSunDir",       SunDir.normalized());
        mShader->setUniformValue("uSunColor",      QVector3D(1.0f, 0.95f, 0.85f));
        mShader->setUniformValue("uAmbientColor",  QVector3D(0.15f, 0.18f, 0.25f));

        // ── Draw each tile ────────────────────────────────────────────────────
        for (const GlobeTerrainTile* Tile : mRenderList)
        {
            if (!Tile->IsReady()) continue;

            // Tile centre offset relative to camera origin (float precision)
            const QVector3D Offset(
                static_cast<float>(Tile->CenterEcef[0] - EC[0]),
                static_cast<float>(Tile->CenterEcef[1] - EC[1]),
                static_cast<float>(Tile->CenterEcef[2] - EC[2])
            );
            mShader->setUniformValue("uTileCenterOffset", Offset);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, Tile->Texture);

            glBindVertexArray(Tile->Vao);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Tile->IndexCount),
                           GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

        mShader->release();

        if (mWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

} // namespace Canavar::Globe
