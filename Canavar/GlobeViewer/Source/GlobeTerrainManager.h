#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  GlobeTerrainManager
//  Each frame:
//    1. Traverses the quadtree index and selects tiles to render (LOD selection
//       based on screen-space error / distance).
//    2. Requests async loading for visible tiles not yet in the GPU cache.
//    3. Uploads completed load results to GPU.
//    4. Draws all ready tiles.
// ─────────────────────────────────────────────────────────────────────────────

#include "TerrainCommon.h"
#include "QuadtreeIndex.h"
#include "GlobeTerrainTile.h"
#include "GlobeTerrainLoader.h"
#include "GlobeCamera.h"

#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QString>
#include <unordered_map>
#include <vector>

namespace Canavar::Globe
{
    class GlobeTerrainManager : protected QOpenGLFunctions_4_5_Core
    {
      public:
        explicit GlobeTerrainManager(const QString& TerrainRoot);
        ~GlobeTerrainManager();

        // Must be called once, on the GL thread with a current context.
        bool Initialize();

        // Render one frame. Called every frame on the GL thread.
        void Render(const GlobeCamera& Camera, int ViewportW, int ViewportH);

        // Accessors for runtime settings
        float GetPixelErrorThreshold() const { return mPixelErrorThreshold; }
        void  SetPixelErrorThreshold(float Threshold) { mPixelErrorThreshold = Threshold; }

        bool GetWireframe() const { return mWireframe; }
        void SetWireframe(bool Enabled) { mWireframe = Enabled; }

        int  GetLoadedTileCount() const { return static_cast<int>(mTileCache.size()); }

      private:
        // Select tiles to render for the current camera; fill mRenderList.
        void SelectTiles(const GlobeCamera& Camera, float ViewportH, float FovRad);

        void TraverseNode(int NodeIdx,
                          const GlobeCamera& Camera,
                          float ViewportH, float FovRad);

        // Upload one completed load result to GPU.
        void UploadResult(std::unique_ptr<TileLoadResult> Result);

        // Release GPU resources of one tile.
        void ReleaseTile(GlobeTerrainTile& Tile);

        // Screen-space error metric (pixels).
        float ScreenSpaceError(const TerrainIndexNode& Node,
                               const GlobeCamera& Camera,
                               float ViewportH, float FovRad) const;

        QString                mTerrainRoot;
        QuadtreeIndex          mIndex;
        GlobeTerrainLoader     mLoader;

        std::unordered_map<TileId, GlobeTerrainTilePtr> mTileCache;
        std::vector<GlobeTerrainTile*>                   mRenderList; // tiles to draw this frame

        std::unique_ptr<QOpenGLShaderProgram> mShader;

        float mPixelErrorThreshold{ 4.0f };
        bool  mWireframe{ false };
        bool  mInitialized{ false };
    };

} // namespace Canavar::Globe
