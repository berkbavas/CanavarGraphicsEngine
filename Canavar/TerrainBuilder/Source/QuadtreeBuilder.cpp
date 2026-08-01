#include "QuadtreeBuilder.h"
#include "ElevationSampler.h"
#include "MeshBuilder.h"
#include "TexturePacker.h"
#include "TileFileWriter.h"

#include <QDir>

namespace Canavar::Globe
{
    QuadtreeBuilder::QuadtreeBuilder(QObject* Parent)
        : QObject(Parent)
    {}

    void QuadtreeBuilder::CollectTiles(const BuildSettings& Settings,
                                        QVector<TileId>&     TilesOut)
    {
        TilesOut.clear();
        for (int Level = Settings.MinLevel; Level <= Settings.MaxLevel; ++Level)
        {
            const double W = TileWidthDeg(Level);
            const double H = TileHeightDeg(Level);

            // Iterate tiles that intersect the requested region
            const int XMin = static_cast<int>((Settings.Region.MinLon + 180.0) / W);
            const int XMax = static_cast<int>((Settings.Region.MaxLon + 180.0) / W);
            const int YMin = static_cast<int>((Settings.Region.MinLat +  90.0) / H);
            const int YMax = static_cast<int>((Settings.Region.MaxLat +  90.0) / H);
            const int MaxN = (1 << Level) - 1;

            for (int X = qMax(0, XMin); X <= qMin(MaxN, XMax); ++X)
                for (int Y = qMax(0, YMin); Y <= qMin(MaxN, YMax); ++Y)
                    TilesOut.append({ Level, X, Y });
        }
    }

    bool QuadtreeBuilder::BuildOneTile(const TileId&       Id,
                                       const BuildSettings& Settings,
                                       ElevationSampler&    Sampler,
                                       TexturePacker&       Packer)
    {
        const GeoBounds Bounds = BoundsForTile(Id);

        MeshBuilder Mesh(Settings.MeshGridRes);
        QVector<TerrainVertex> Vertices;
        QVector<uint32_t>      Indices;
        std::array<double, 3>  CenterEcef;

        if (!Mesh.Build(Bounds, Sampler, Vertices, Indices, CenterEcef))
            return false;

        // Cap imagery zoom so that each terrain tile accesses at most ~16x16 imagery tiles.
        // At level L, EffectiveZoom = L+4 gives ~16 imagery tiles per terrain tile side.
        const int EffectiveZoom = qMin(Settings.ImageryZoom, Id.Level + 4);
        QImage Texture = Packer.Pack(Bounds, EffectiveZoom);

        TileFileWriter Writer(Settings.OutputFolder, Settings.JpegQuality);
        return Writer.Write(Id, Bounds, CenterEcef, Vertices, Indices, Texture);
    }

    bool QuadtreeBuilder::Build(const BuildSettings& Settings)
    {
        mAbortRequested = false;

        QDir OutputDir(Settings.OutputFolder);
        OutputDir.mkpath(".");

        // ── collect all tiles ─────────────────────────────────────────────────
        QVector<TileId> Tiles;
        CollectTiles(Settings, Tiles);

        // Create shared objects once per build to avoid redundant work.
        ElevationSampler Sampler(Settings.DtedFolder, 16);
        Sampler.ScanDtedFolder();

        TexturePacker Packer(Settings.AlbedoFolder, Settings.ImageryZoom, Settings.TextureSize);

        emit ProgressMessage(QString("Building %1 tiles (L%2-L%3)...")
                             .arg(Tiles.size())
                             .arg(Settings.MinLevel)
                             .arg(Settings.MaxLevel));

        TileFileWriter Writer(Settings.OutputFolder, Settings.JpegQuality);
        QuadtreeIndex  Index;

        int Done = 0;
        for (const TileId& Id : Tiles)
        {
            if (mAbortRequested)
            {
                emit ProgressMessage("Build aborted by user.");
                emit BuildFinished(false);
                return false;
            }

            const bool Skip = Settings.SkipExisting && Writer.TileExists(Id);

            if (!Skip)
            {
                emit ProgressMessage(QString("Building L%1/X%2/Y%3...")
                                     .arg(Id.Level).arg(Id.X).arg(Id.Y));

                if (!BuildOneTile(Id, Settings, Sampler, Packer))
                {
                    emit ProgressMessage(
                        QString("  Warning: failed tile L%1/X%2/Y%3")
                        .arg(Id.Level).arg(Id.X).arg(Id.Y));
                }
            }

            // Register tile in the index (even skipped/failed ones may already exist)
            const GeoBounds Bounds = BoundsForTile(Id);
            const auto Center = GeodeticToECEF(Bounds.LatCenter(), Bounds.LonCenter(), 0.0);

            TerrainIndexNode Node{};
            Node.TileLevel    = Id.Level;
            Node.TileX        = Id.X;
            Node.TileY        = Id.Y;
            Node.MinLat       = Bounds.MinLat;
            Node.MaxLat       = Bounds.MaxLat;
            Node.MinLon       = Bounds.MinLon;
            Node.MaxLon       = Bounds.MaxLon;
            Node.CenterEcefX  = Center[0];
            Node.CenterEcefY  = Center[1];
            Node.CenterEcefZ  = Center[2];
            Node.GeometricError = static_cast<float>(
                Bounds.LatSpan() * 111000.0 / Settings.MeshGridRes); // ≈ metres per vertex
            Node.HasData      = Writer.TileExists(Id) ? 1u : 0u;
            Node.ChildMask    = (Id.Level < Settings.MaxLevel) ? 0x0F : 0x00;
            Node.Padding[0]   = Node.Padding[1] = 0;
            Node.ChildIndices[0] = Node.ChildIndices[1] =
            Node.ChildIndices[2] = Node.ChildIndices[3] = -1;

            Index.AddNode(Node);

            ++Done;
            emit TileCompleted(Done, Tiles.size());
        }

        // ── link child indices in the index ────────────────────────────────────
        const int NodeCount = Index.NodeCount();
        for (int I = 0; I < NodeCount; ++I)
        {
            auto& Node = Index.GetNode(I);
            if (!Node.ChildMask) continue;

            for (int Q = 0; Q < 4; ++Q)
            {
                const TileId Child = ChildTile({ Node.TileLevel, Node.TileX, Node.TileY }, Q);
                const int ChildIdx = Index.FindNode(Child);
                Node.ChildIndices[Q] = ChildIdx;
            }
        }

        // ── save index ────────────────────────────────────────────────────────
        const QString IndexPath = Settings.OutputFolder + "/index.tidx";
        if (!Index.Save(IndexPath))
        {
            emit ProgressMessage("ERROR: Failed to save index file: " + IndexPath);
            emit BuildFinished(false);
            return false;
        }

        emit ProgressMessage(QString("Build complete. %1 tiles processed.").arg(Done));
        emit BuildFinished(true);
        return true;
    }

} // namespace Canavar::Globe
