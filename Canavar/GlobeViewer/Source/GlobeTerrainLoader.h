#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  GlobeTerrainLoader
//  Asynchronously loads .tbin files from disk into CPU-side data,
//  then queues GPU upload to the main thread.
// ─────────────────────────────────────────────────────────────────────────────

#include "TerrainCommon.h"
#include "GlobeTerrainTile.h"

#include <QImage>
#include <QObject>
#include <QRunnable>
#include <QThreadPool>
#include <QMutex>
#include <unordered_map>
#include <unordered_set>
#include <deque>

namespace Canavar::Globe
{
    // CPU-side data ready for GPU upload
    struct TileLoadResult
    {
        TileId                        Id;
        GeoBounds                     Bounds;
        std::array<double, 3>         CenterEcef{};
        float                         GeometricError{ 0 };
        QVector<TerrainVertex>        Vertices;
        QVector<uint32_t>             Indices;
        QImage                        Texture;      // decoded JPEG
        bool                          Success{ false };
    };

    class GlobeTerrainLoader : public QObject
    {
        Q_OBJECT

      public:
        explicit GlobeTerrainLoader(const QString& TerrainRoot, QObject* Parent = nullptr);
        ~GlobeTerrainLoader() override;

        // Request asynchronous loading. No-op if already in-flight or loaded.
        void RequestLoad(const TileId& Id);

        // Cancel a pending request (has no effect if already in-flight).
        void CancelLoad(const TileId& Id);

        // Poll: dequeue one completed result. Returns null if none ready.
        std::unique_ptr<TileLoadResult> PollResult();

        bool IsPending(const TileId& Id) const;

        // Called by TileLoadTask (worker thread) to deliver a completed result.
        void DeliverResult(std::unique_ptr<TileLoadResult> Result);

      private:
        QString mTerrainRoot;

        mutable QMutex mMutex;
        std::unordered_set<TileId>                      mPending;  // in-flight
        std::deque<std::unique_ptr<TileLoadResult>>      mResults;  // ready for GPU upload
        QThreadPool mPool;
    };

} // namespace Canavar::Globe
