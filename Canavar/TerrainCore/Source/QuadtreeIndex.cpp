#include "QuadtreeIndex.h"

#include <QDataStream>
#include <QFile>

namespace Canavar::Globe
{
    bool QuadtreeIndex::Load(const QString& FilePath)
    {
        QFile File(FilePath);
        if (!File.open(QIODevice::ReadOnly))
            return false;

        QDataStream Stream(&File);
        Stream.setByteOrder(QDataStream::LittleEndian);
        Stream.setFloatingPointPrecision(QDataStream::DoublePrecision);

        TerrainIndexHeader Header{};
        Stream.readRawData(reinterpret_cast<char*>(&Header), sizeof(Header));

        if (Header.Magic   != IndexMagic)   return false;
        if (Header.Version != IndexVersion) return false;

        mMinLevel = Header.MinLevel;
        mMaxLevel = Header.MaxLevel;

        mNodes.resize(static_cast<int>(Header.NodeCount));
        Stream.readRawData(
            reinterpret_cast<char*>(mNodes.data()),
            static_cast<int>(Header.NodeCount * sizeof(TerrainIndexNode)));

        return Stream.status() == QDataStream::Ok;
    }

    bool QuadtreeIndex::Save(const QString& FilePath) const
    {
        QFile File(FilePath);
        if (!File.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return false;

        QDataStream Stream(&File);
        Stream.setByteOrder(QDataStream::LittleEndian);

        TerrainIndexHeader Header{};
        Header.Magic     = IndexMagic;
        Header.Version   = IndexVersion;
        Header.MinLevel  = mMinLevel;
        Header.MaxLevel  = mMaxLevel;
        Header.NodeCount = static_cast<uint32_t>(mNodes.size());

        Stream.writeRawData(reinterpret_cast<const char*>(&Header), sizeof(Header));
        Stream.writeRawData(
            reinterpret_cast<const char*>(mNodes.constData()),
            static_cast<int>(mNodes.size() * sizeof(TerrainIndexNode)));

        return Stream.status() == QDataStream::Ok;
    }

    int QuadtreeIndex::AddNode(const TerrainIndexNode& Node)
    {
        const int Idx = static_cast<int>(mNodes.size());
        mNodes.append(Node);

        if (Node.TileLevel < mMinLevel) mMinLevel = Node.TileLevel;
        if (Node.TileLevel > mMaxLevel) mMaxLevel = Node.TileLevel;

        return Idx;
    }

    int QuadtreeIndex::FindNode(const TileId& Id) const
    {
        for (int I = 0; I < mNodes.size(); ++I)
        {
            const auto& N = mNodes[I];
            if (N.TileLevel == Id.Level && N.TileX == Id.X && N.TileY == Id.Y)
                return I;
        }
        return -1;
    }

    QVector<int> QuadtreeIndex::RootNodeIndices() const
    {
        QVector<int> Result;
        for (int I = 0; I < mNodes.size(); ++I)
            if (mNodes[I].TileLevel == mMinLevel)
                Result.append(I);
        return Result;
    }

} // namespace Canavar::Globe
