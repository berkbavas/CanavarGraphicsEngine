#pragma once

#include "TerrainCommon.h"

#include <QString>
#include <QVector>

namespace Canavar::Globe
{
    // Loaded representation of the quadtree index file.
    class QuadtreeIndex
    {
      public:
        QuadtreeIndex() = default;

        // Load from .tidx file. Returns true on success.
        bool Load(const QString& FilePath);

        // Save to .tidx file. Returns true on success.
        bool Save(const QString& FilePath) const;

        // Add a node (used by the builder). Returns the new node's array index.
        int AddNode(const TerrainIndexNode& Node);

        // Find the index of a node by TileId. Returns -1 if not found.
        int FindNode(const TileId& Id) const;

        const TerrainIndexNode& GetNode(int Index) const { return mNodes[Index]; }
        TerrainIndexNode&       GetNode(int Index)       { return mNodes[Index]; }

        int NodeCount() const { return static_cast<int>(mNodes.size()); }

        int MinLevel() const { return mMinLevel; }
        int MaxLevel() const { return mMaxLevel; }

        // Root nodes (level == MinLevel)
        QVector<int> RootNodeIndices() const;

        void Clear() { mNodes.clear(); mMinLevel = 99; mMaxLevel = -1; }

        const QVector<TerrainIndexNode>& Nodes() const { return mNodes; }

      private:
        QVector<TerrainIndexNode> mNodes;
        int mMinLevel{ 99 };
        int mMaxLevel{ -1 };
    };

} // namespace Canavar::Globe
