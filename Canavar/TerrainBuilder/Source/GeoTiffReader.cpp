#include "GeoTiffReader.h"

#include <QFile>
#include <QDataStream>

namespace Canavar::Globe
{
    // ─── TIFF tag constants ───────────────────────────────────────────────────
    static constexpr quint16 TAG_IMAGE_WIDTH        = 256;
    static constexpr quint16 TAG_IMAGE_LENGTH       = 257;
    static constexpr quint16 TAG_BITS_PER_SAMPLE    = 258;
    static constexpr quint16 TAG_COMPRESSION        = 259;
    static constexpr quint16 TAG_ROWS_PER_STRIP     = 278;
    static constexpr quint16 TAG_STRIP_OFFSETS      = 273;
    static constexpr quint16 TAG_STRIP_BYTE_COUNTS  = 279;
    static constexpr quint16 TAG_TILE_WIDTH         = 322;
    static constexpr quint16 TAG_TILE_LENGTH        = 323;
    static constexpr quint16 TAG_TILE_OFFSETS       = 324;
    static constexpr quint16 TAG_TILE_BYTE_COUNTS   = 325;
    static constexpr quint16 TAG_MODEL_PIXEL_SCALE  = 33550; // GeoTIFF
    static constexpr quint16 TAG_MODEL_TIEPOINT     = 33922; // GeoTIFF

    // TIFF type sizes in bytes
    static const quint32 kTiffTypeSizes[] = { 0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8, 4, 8 };

    static quint32 TiffTypeSize(quint16 Type)
    {
        if (Type == 0 || Type >= 15) return 1;
        return kTiffTypeSizes[Type];
    }

    // ─────────────────────────────────────────────────────────────────────────

    bool GeoTiffReader::Open(const QString& FilePath)
    {
        mFilePath = FilePath;

        QFile File(FilePath);
        if (!File.open(QIODevice::ReadOnly)) return false;

        QDataStream Stream(&File);

        // Byte order
        quint16 ByteOrder = 0;
        Stream >> ByteOrder;
        if (ByteOrder == 0x4949u)       // "II" little-endian
            Stream.setByteOrder(QDataStream::LittleEndian);
        else if (ByteOrder == 0x4D4Du)  // "MM" big-endian
            Stream.setByteOrder(QDataStream::BigEndian);
        else
            return false;

        quint16 Magic = 0;
        Stream >> Magic;
        if (Magic != 42) return false; // only classic TIFF

        quint32 IfdOffset = 0;
        Stream >> IfdOffset;

        return ParseIfd(static_cast<qint64>(IfdOffset));
    }

    bool GeoTiffReader::ParseIfd(qint64 Offset)
    {
        QFile File(mFilePath);
        if (!File.open(QIODevice::ReadOnly)) return false;
        if (!File.seek(Offset)) return false;

        QDataStream Stream(&File);
        Stream.setByteOrder(QDataStream::LittleEndian);

        quint16 EntryCount = 0;
        Stream >> EntryCount;

        for (quint16 E = 0; E < EntryCount; ++E)
        {
            quint16 Tag{0}, Type{0};
            quint32 Count{0}, ValueOrOffset{0};
            Stream >> Tag >> Type >> Count >> ValueOrOffset;

            switch (Tag)
            {
                case TAG_IMAGE_WIDTH:
                    mWidth = (Type == 3) ? (ValueOrOffset & 0xFFFF) : ValueOrOffset;
                    break;

                case TAG_IMAGE_LENGTH:
                    mHeight = (Type == 3) ? (ValueOrOffset & 0xFFFF) : ValueOrOffset;
                    break;

                case TAG_BITS_PER_SAMPLE:
                    mBitsPerSample = (Type == 3) ? static_cast<quint16>(ValueOrOffset & 0xFFFF) : 16;
                    break;

                case TAG_COMPRESSION:
                    mCompression = static_cast<quint16>(ValueOrOffset & 0xFFFF);
                    break;

                case TAG_ROWS_PER_STRIP:
                    mRowsPerStrip = (Type == 3) ? (ValueOrOffset & 0xFFFF) : ValueOrOffset;
                    break;

                case TAG_STRIP_OFFSETS:
                    mStripOffsets = ReadTagValues(Type, Count, ValueOrOffset);
                    break;

                case TAG_STRIP_BYTE_COUNTS:
                    mStripByteCounts = ReadTagValues(Type, Count, ValueOrOffset);
                    break;

                case TAG_TILE_WIDTH:
                    mTileWidth = (Type == 3) ? (ValueOrOffset & 0xFFFF) : ValueOrOffset;
                    mUseTiles = true;
                    break;

                case TAG_TILE_LENGTH:
                    mTileHeight = (Type == 3) ? (ValueOrOffset & 0xFFFF) : ValueOrOffset;
                    break;

                case TAG_TILE_OFFSETS:
                    mTileOffsets = ReadTagValues(Type, Count, ValueOrOffset);
                    mUseTiles = true;
                    break;

                case TAG_TILE_BYTE_COUNTS:
                    mTileByteCounts = ReadTagValues(Type, Count, ValueOrOffset);
                    break;

                case TAG_MODEL_PIXEL_SCALE:
                {
                    // 3 doubles: ScaleX (lon per pixel), ScaleY (lat per pixel), ScaleZ
                    const qint64 SavePos = File.pos();
                    if (File.seek(static_cast<qint64>(ValueOrOffset)))
                    {
                        QDataStream GeoStream(&File);
                        GeoStream.setByteOrder(QDataStream::LittleEndian);
                        GeoStream.setFloatingPointPrecision(QDataStream::DoublePrecision);
                        GeoStream >> mPixelScaleLon >> mPixelScaleLat;
                        mHasGeoTags = true;
                    }
                    File.seek(SavePos);
                    break;
                }

                case TAG_MODEL_TIEPOINT:
                {
                    // 6 doubles: I, J, K, X (lon), Y (lat), Z
                    const qint64 SavePos = File.pos();
                    if (File.seek(static_cast<qint64>(ValueOrOffset)))
                    {
                        QDataStream GeoStream(&File);
                        GeoStream.setByteOrder(QDataStream::LittleEndian);
                        GeoStream.setFloatingPointPrecision(QDataStream::DoublePrecision);
                        double I{0}, J{0}, K{0};
                        GeoStream >> I >> J >> K >> mOriginLon >> mOriginLat;
                        mHasGeoTags = true;
                    }
                    File.seek(SavePos);
                    break;
                }

                default: break;
            }
        }

        return (mWidth > 0 && mHeight > 0);
    }

    QVector<quint64> GeoTiffReader::ReadTagValues(quint16 TagType, quint32 Count, quint32 ValueOrOffset)
    {
        const quint32 TypeSize = TiffTypeSize(TagType);
        const quint32 TotalBytes = Count * TypeSize;
        QVector<quint64> Result;
        Result.reserve(static_cast<int>(Count));

        auto ReadFromBuffer = [&](const char* Buf)
        {
            for (quint32 I = 0; I < Count; ++I)
            {
                quint64 Val = 0;
                if (TypeSize == 2)
                {
                    quint16 V = 0;
                    memcpy(&V, Buf + I * TypeSize, 2);
                    Val = V;
                }
                else if (TypeSize == 4)
                {
                    quint32 V = 0;
                    memcpy(&V, Buf + I * TypeSize, 4);
                    Val = V;
                }
                else if (TypeSize == 8)
                {
                    memcpy(&Val, Buf + I * TypeSize, 8);
                }
                else
                    Val = 0;
                Result.append(Val);
            }
        };

        if (TotalBytes <= 4)
        {
            // Value fits in the IFD entry itself
            char Buf[4];
            memcpy(Buf, &ValueOrOffset, 4);
            ReadFromBuffer(Buf);
        }
        else
        {
            // Value is at an offset
            QFile File(mFilePath);
            if (File.open(QIODevice::ReadOnly) && File.seek(static_cast<qint64>(ValueOrOffset)))
            {
                QByteArray Data = File.read(static_cast<qint64>(TotalBytes));
                if (Data.size() == static_cast<int>(TotalBytes))
                    ReadFromBuffer(Data.constData());
            }
        }

        return Result;
    }

    QByteArray GeoTiffReader::DeflateDecompress(qint64 FileOffset,
                                                 quint32 CompressedSize,
                                                 quint32 UncompressedSize)
    {
        QFile File(mFilePath);
        if (!File.open(QIODevice::ReadOnly) || !File.seek(FileOffset)) return {};

        QByteArray Compressed = File.read(static_cast<qint64>(CompressedSize));
        if (Compressed.size() != static_cast<int>(CompressedSize)) return {};

        // qUncompress expects: 4-byte big-endian uncompressed size followed by zlib data
        QByteArray Header(4, '\0');
        Header[0] = static_cast<char>((UncompressedSize >> 24) & 0xFF);
        Header[1] = static_cast<char>((UncompressedSize >> 16) & 0xFF);
        Header[2] = static_cast<char>((UncompressedSize >> 8)  & 0xFF);
        Header[3] = static_cast<char>( UncompressedSize        & 0xFF);
        Header.append(Compressed);

        return qUncompress(Header);
    }

    void GeoTiffReader::CopyTileToGrid(const QVector<qint16>& TileData,
                                       uint32_t TileCol, uint32_t TileRow,
                                       QVector<qint16>& Grid)
    {
        const uint32_t DestStartX = TileCol * mTileWidth;
        const uint32_t DestStartY = TileRow * mTileHeight;

        const uint32_t CopyW = qMin(mTileWidth,  mWidth  - DestStartX);
        const uint32_t CopyH = qMin(mTileHeight, mHeight - DestStartY);

        for (uint32_t Row = 0; Row < CopyH; ++Row)
        {
            const uint32_t DestIdx = (DestStartY + Row) * mWidth + DestStartX;
            const uint32_t SrcIdx  = Row * mTileWidth;
            memcpy(Grid.data() + DestIdx, TileData.constData() + SrcIdx, CopyW * sizeof(qint16));
        }
    }

    bool GeoTiffReader::ReadAll(QVector<qint16>& ElevationOut)
    {
        if (mWidth == 0 || mHeight == 0) return false;

        ElevationOut.resize(static_cast<int>(mWidth * mHeight));

        const uint32_t BytesPerSample = mBitsPerSample / 8u;

        if (mUseTiles)
        {
            const uint32_t TilesAcross = (mWidth  + mTileWidth  - 1u) / mTileWidth;
            const uint32_t TilesDown   = (mHeight + mTileHeight - 1u) / mTileHeight;
            const uint32_t UncompBytes = mTileWidth * mTileHeight * BytesPerSample;

            for (uint32_t TRow = 0; TRow < TilesDown; ++TRow)
            {
                for (uint32_t TCol = 0; TCol < TilesAcross; ++TCol)
                {
                    const uint32_t TileIdx = TRow * TilesAcross + TCol;
                    if (TileIdx >= static_cast<uint32_t>(mTileOffsets.size())) return false;

                    const quint64 Offset     = mTileOffsets[TileIdx];
                    const quint32 ByteCount  = static_cast<quint32>(mTileByteCounts.value(TileIdx));

                    QByteArray RawBytes;

                    if (mCompression == 8 || mCompression == 32946)
                    {
                        RawBytes = DeflateDecompress(static_cast<qint64>(Offset),
                                                     ByteCount, UncompBytes);
                    }
                    else // uncompressed
                    {
                        QFile File(mFilePath);
                        if (!File.open(QIODevice::ReadOnly) || !File.seek(static_cast<qint64>(Offset)))
                            return false;
                        RawBytes = File.read(static_cast<qint64>(UncompBytes));
                    }

                    if (RawBytes.isEmpty()) return false;

                    QVector<qint16> TileInts(static_cast<int>(mTileWidth * mTileHeight));
                    memcpy(TileInts.data(), RawBytes.constData(),
                           static_cast<size_t>(TileInts.size()) * sizeof(qint16));

                    CopyTileToGrid(TileInts, TCol, TRow, ElevationOut);
                }
            }
        }
        else
        {
            // Strip organisation
            const uint32_t RowsPS   = (mRowsPerStrip > 0) ? mRowsPerStrip : mHeight;
            const uint32_t NumStrips = (mHeight + RowsPS - 1) / RowsPS;

            for (uint32_t S = 0; S < NumStrips; ++S)
            {
                const uint32_t StartRow   = S * RowsPS;
                const uint32_t StripRows  = qMin(RowsPS, mHeight - StartRow);
                const uint32_t UncompBytes = StripRows * mWidth * BytesPerSample;
                const quint64  Offset      = mStripOffsets.value(S);
                const quint32  ByteCount   = static_cast<quint32>(mStripByteCounts.value(S));

                QByteArray RawBytes;

                if (mCompression == 8 || mCompression == 32946)
                {
                    RawBytes = DeflateDecompress(static_cast<qint64>(Offset),
                                                 ByteCount, UncompBytes);
                }
                else
                {
                    QFile File(mFilePath);
                    if (!File.open(QIODevice::ReadOnly) || !File.seek(static_cast<qint64>(Offset)))
                        return false;
                    RawBytes = File.read(static_cast<qint64>(UncompBytes));
                }

                if (RawBytes.isEmpty()) return false;

                const qint16* Src  = reinterpret_cast<const qint16*>(RawBytes.constData());
                qint16*       Dest = ElevationOut.data() + StartRow * mWidth;
                memcpy(Dest, Src, StripRows * mWidth * sizeof(qint16));
            }
        }

        return true;
    }

} // namespace Canavar::Globe
