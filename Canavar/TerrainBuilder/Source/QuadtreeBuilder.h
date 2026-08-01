#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  QuadtreeBuilder
//  Orchestrates the offline terrain build pipeline:
//    1. Scan DTED and imagery folders
//    2. Build quadtree structure for a geographic region
//    3. For each tile: generate mesh + texture → write .tbin
//    4. Write .tidx quadtree index
// ─────────────────────────────────────────────────────────────────────────────

#include "TerrainCommon.h"
#include "QuadtreeIndex.h"
#include "ElevationSampler.h"
#include "TexturePacker.h"

#include <QObject>
#include <QString>
#include <functional>

namespace Canavar::Globe
{
    struct BuildSettings
    {
        QString DtedFolder;    // GLO-30 .tif files
        QString AlbedoFolder;  // XYZ imagery tiles root
        QString OutputFolder;  // where to write .tbin files and index.tidx
        GeoBounds Region;      // geographic region to build
        int MinLevel{ 2 };     // coarsest LOD level to build
        int MaxLevel{ 8 };     // finest  LOD level to build
        int MeshGridRes{ 65 }; // vertices per side (65 = 64×64 quads)
        int ImageryZoom{ 12 }; // imagery zoom level used for textures
        int TextureSize{ 512 };// output texture size in pixels
        int JpegQuality{ 85 };
        bool SkipExisting{ true };
    };

    class QuadtreeBuilder : public QObject
    {
        Q_OBJECT

      public:
        explicit QuadtreeBuilder(QObject* Parent = nullptr);

        // Start a synchronous build. Progress is emitted via signals.
        // Call from a worker thread.
        bool Build(const BuildSettings& Settings);

        void RequestAbort() { mAbortRequested = true; }
        bool IsAborted() const { return mAbortRequested; }

      signals:
        void ProgressMessage(const QString& Message);
        void TileCompleted(int Done, int Total);
        void BuildFinished(bool Success);

      private:
        void CollectTiles(const BuildSettings& Settings,
                          QVector<TileId>&     TilesOut);

        bool BuildOneTile(const TileId&       Id,
                          const BuildSettings& Settings,
                          ElevationSampler&    Sampler,
                          TexturePacker&       Packer);

        bool mAbortRequested{ false };
    };

} // namespace Canavar::Globe
