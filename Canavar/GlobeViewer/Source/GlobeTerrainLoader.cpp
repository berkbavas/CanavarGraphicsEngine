#include "GlobeTerrainLoader.h"

#include <QFile>
#include <QImage>
#include <QMutexLocker>
#include <QRunnable>

namespace Canavar::Globe
{
    // ── Worker runnable ───────────────────────────────────────────────────────

    class TileLoadTask : public QRunnable
    {
      public:
        TileLoadTask(const TileId&         Id,
                     const QString&        TerrainRoot,
                     GlobeTerrainLoader*   Loader)
            : mId(Id), mRoot(TerrainRoot), mLoader(Loader)
        {
            setAutoDelete(true);
        }

        void run() override
        {
            auto Result = std::make_unique<TileLoadResult>();
            Result->Id = mId;

            const QString Path =
                QString("%1/L%2/X%3/Y%4.tbin")
                    .arg(mRoot).arg(mId.Level).arg(mId.X).arg(mId.Y);

            QFile File(Path);
            if (!File.open(QIODevice::ReadOnly))
            {
                mLoader->DeliverResult(std::move(Result));
                return;
            }

            // Read header
            TerrainTileHeader Header{};
            if (File.read(reinterpret_cast<char*>(&Header), sizeof(Header))
                    != static_cast<qint64>(sizeof(Header)))
            {
                mLoader->DeliverResult(std::move(Result));
                return;
            }

            if (Header.Magic != TileMagic || Header.Version != TileVersion)
            {
                mLoader->DeliverResult(std::move(Result));
                return;
            }

            // Read vertices
            Result->Vertices.resize(static_cast<int>(Header.VertexCount));
            File.read(reinterpret_cast<char*>(Result->Vertices.data()),
                      static_cast<qint64>(Header.VertexCount * sizeof(TerrainVertex)));

            // Read indices
            Result->Indices.resize(static_cast<int>(Header.IndexCount));
            File.read(reinterpret_cast<char*>(Result->Indices.data()),
                      static_cast<qint64>(Header.IndexCount * sizeof(uint32_t)));

            // Read and decode texture JPEG
            const QByteArray JpegData = File.read(static_cast<qint64>(Header.TextureDataSize));
            Result->Texture = QImage::fromData(JpegData, "JPEG");

            Result->Bounds       = { Header.MinLat,     Header.MaxLat,
                                     Header.MinLon,     Header.MaxLon };
            Result->CenterEcef   = { Header.CenterEcefX, Header.CenterEcefY, Header.CenterEcefZ };
            Result->GeometricError = static_cast<float>(Header.MaxLat - Header.MinLat) * 111000.0f;
            Result->Success        = !Result->Texture.isNull();

            mLoader->DeliverResult(std::move(Result));
        }

      private:
        TileId              mId;
        QString             mRoot;
        GlobeTerrainLoader* mLoader;
    };

    // ── GlobeTerrainLoader ────────────────────────────────────────────────────

    GlobeTerrainLoader::GlobeTerrainLoader(const QString& TerrainRoot, QObject* Parent)
        : QObject(Parent), mTerrainRoot(TerrainRoot)
    {
        mPool.setMaxThreadCount(4);
    }

    GlobeTerrainLoader::~GlobeTerrainLoader()
    {
        mPool.waitForDone(3000);
    }

    void GlobeTerrainLoader::RequestLoad(const TileId& Id)
    {
        QMutexLocker Lock(&mMutex);
        if (mPending.count(Id)) return;
        mPending.insert(Id);
        mPool.start(new TileLoadTask(Id, mTerrainRoot, this));
    }

    void GlobeTerrainLoader::CancelLoad(const TileId& Id)
    {
        // Tasks are already running or queued; mark as cancelled via removing from pending.
        // The result will still arrive but the manager will ignore it.
        QMutexLocker Lock(&mMutex);
        mPending.erase(Id);
    }

    bool GlobeTerrainLoader::IsPending(const TileId& Id) const
    {
        QMutexLocker Lock(&mMutex);
        return mPending.count(Id) > 0;
    }

    std::unique_ptr<TileLoadResult> GlobeTerrainLoader::PollResult()
    {
        QMutexLocker Lock(&mMutex);
        if (mResults.empty()) return nullptr;
        auto Result = std::move(mResults.front());
        mResults.pop_front();
        return Result;
    }

    void GlobeTerrainLoader::DeliverResult(std::unique_ptr<TileLoadResult> Result)
    {
        QMutexLocker Lock(&mMutex);
        mPending.erase(Result->Id);
        mResults.push_back(std::move(Result));
    }

} // namespace Canavar::Globe
