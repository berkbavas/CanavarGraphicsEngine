#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  GeoTiffReader
//  Minimal reader for Copernicus GLO-30 Cloud-Optimised GeoTIFF files.
//  Supports:
//    • Classic TIFF (magic 42), little-endian
//    • INT16 single-channel samples
//    • DEFLATE compression (tag 259 = 8) via qUncompress()
//    • Tiled and stripped image organisation
// ─────────────────────────────────────────────────────────────────────────────

#include <QVector>
#include <QString>

namespace Canavar::Globe
{
    class GeoTiffReader
    {
      public:
        GeoTiffReader() = default;

        // Open and parse the IFD headers. Does NOT read pixel data yet.
        bool Open(const QString& FilePath);

        // Read the entire elevation grid into a flat INT16 buffer.
        // Buffer layout: row-major, origin at TOP-LEFT (north–west corner).
        // Width() × Height() elements.
        bool ReadAll(QVector<qint16>& ElevationOut);

        uint32_t Width()  const { return mWidth; }
        uint32_t Height() const { return mHeight; }

        // Geographic bounds derived from TIFF geo tags (if available).
        // Falls back to invalid doubles – caller should derive from filename.
        double OriginLon()    const { return mOriginLon; }    // longitude of left edge
        double OriginLat()    const { return mOriginLat; }    // latitude of TOP edge
        double PixelScaleLon() const { return mPixelScaleLon; }
        double PixelScaleLat() const { return mPixelScaleLat; }
        bool   HasGeoTags()   const { return mHasGeoTags; }

      private:
        bool ParseIfd(qint64 Offset);

        // Read a multi-value TIFF tag from the file (handles in-entry and offset storage).
        QVector<quint64> ReadTagValues(quint16 TagType, quint32 Count, quint32 ValueOrOffset);

        // Decompress one zlib (DEFLATE, compression=8) strip/tile block.
        QByteArray DeflateDecompress(qint64 FileOffset,
                                     quint32 CompressedSize,
                                     quint32 UncompressedSize);

        // Copy a decompressed INT16 tile block into the output grid.
        void CopyTileToGrid(const QVector<qint16>& TileData,
                            uint32_t TileCol, uint32_t TileRow,
                            QVector<qint16>& Grid);

        QString  mFilePath;

        uint32_t mWidth{0},  mHeight{0};
        uint16_t mBitsPerSample{16};
        uint16_t mCompression{1};    // 1=none, 8=deflate

        // Tiled image
        bool     mUseTiles{false};
        uint32_t mTileWidth{0}, mTileHeight{0};
        QVector<quint64> mTileOffsets;
        QVector<quint64> mTileByteCounts;

        // Stripped image
        uint32_t mRowsPerStrip{0};
        QVector<quint64> mStripOffsets;
        QVector<quint64> mStripByteCounts;

        // Geo tags
        bool   mHasGeoTags{false};
        double mOriginLon{0}, mOriginLat{0};
        double mPixelScaleLon{0}, mPixelScaleLat{0};
    };

} // namespace Canavar::Globe
