#include "TileFileWriter.h"

#include <QBuffer>
#include <QDir>
#include <QFile>

namespace Canavar::Globe
{
    TileFileWriter::TileFileWriter(const QString& OutputRoot, int JpegQuality)
        : mOutputRoot(OutputRoot), mJpegQuality(JpegQuality)
    {}

    QString TileFileWriter::TilePath(const TileId& Id) const
    {
        return QString("%1/L%2/X%3/Y%4.tbin")
            .arg(mOutputRoot)
            .arg(Id.Level)
            .arg(Id.X)
            .arg(Id.Y);
    }

    bool TileFileWriter::TileExists(const TileId& Id) const
    {
        return QFile::exists(TilePath(Id));
    }

    bool TileFileWriter::Write(const TileId&                 Id,
                                const GeoBounds&              Bounds,
                                const std::array<double, 3>&  CenterEcef,
                                const QVector<TerrainVertex>& Vertices,
                                const QVector<uint32_t>&      Indices,
                                const QImage&                 Texture)
    {
        const QString Path = TilePath(Id);
        QFileInfo Fi(Path);
        Fi.dir().mkpath(".");

        QFile File(Path);
        if (!File.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return false;

        // Encode texture to JPEG in memory
        QByteArray TexJpeg;
        {
            QBuffer Buf(&TexJpeg);
            Buf.open(QIODevice::WriteOnly);
            if (!Texture.save(&Buf, "JPEG", mJpegQuality))
                return false;
        }

        // Build header
        TerrainTileHeader Header{};
        Header.Magic          = TileMagic;
        Header.Version        = TileVersion;
        Header.TileLevel      = Id.Level;
        Header.TileX          = Id.X;
        Header.TileY          = Id.Y;
        Header.MinLat         = Bounds.MinLat;
        Header.MaxLat         = Bounds.MaxLat;
        Header.MinLon         = Bounds.MinLon;
        Header.MaxLon         = Bounds.MaxLon;
        Header.CenterEcefX    = CenterEcef[0];
        Header.CenterEcefY    = CenterEcef[1];
        Header.CenterEcefZ    = CenterEcef[2];
        Header.VertexCount    = static_cast<uint32_t>(Vertices.size());
        Header.IndexCount     = static_cast<uint32_t>(Indices.size());
        Header.TextureDataSize = static_cast<uint32_t>(TexJpeg.size());

        File.write(reinterpret_cast<const char*>(&Header), sizeof(Header));
        File.write(reinterpret_cast<const char*>(Vertices.constData()),
                   static_cast<qint64>(Vertices.size()) * sizeof(TerrainVertex));
        File.write(reinterpret_cast<const char*>(Indices.constData()),
                   static_cast<qint64>(Indices.size()) * sizeof(uint32_t));
        File.write(TexJpeg);

        return File.error() == QFile::NoError;
    }

} // namespace Canavar::Globe
