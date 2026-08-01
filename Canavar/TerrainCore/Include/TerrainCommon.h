#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  TerrainCommon.h
//  Shared types for the WGS-84 quadtree terrain system.
//  Used by both the offline TerrainBuilder and the runtime GlobeViewer.
// ─────────────────────────────────────────────────────────────────────────────

#include <array>
#include <cmath>
#include <cstdint>
#include <functional>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Canavar::Globe
{
    // ─────────────────────────────────────────────────────────────────────────
    //  WGS-84 ellipsoid constants and conversions
    // ─────────────────────────────────────────────────────────────────────────

    inline constexpr double Wgs84A  = 6'378'137.0;                      // Semi-major axis (m)
    inline constexpr double Wgs84F  = 1.0 / 298.257223563;              // Flattening
    inline constexpr double Wgs84B  = Wgs84A * (1.0 - Wgs84F);          // Semi-minor axis (m)
    inline constexpr double Wgs84E2 = 2.0 * Wgs84F - Wgs84F * Wgs84F;  // Eccentricity²

    // Geodetic (lat/lon degrees, altitude metres) → ECEF metres
    inline std::array<double, 3> GeodeticToECEF(double LatDeg, double LonDeg, double AltM = 0.0)
    {
        const double Lr = LatDeg * M_PI / 180.0;
        const double Nr = LonDeg * M_PI / 180.0;
        const double SL = std::sin(Lr), CL = std::cos(Lr);
        const double SN = std::sin(Nr), CN = std::cos(Nr);
        const double N  = Wgs84A / std::sqrt(1.0 - Wgs84E2 * SL * SL);
        return { (N + AltM) * CL * CN,
                 (N + AltM) * CL * SN,
                 (N * (1.0 - Wgs84E2) + AltM) * SL };
    }

    // Surface normal at geodetic position (unit ECEF vector, pointing outward)
    inline std::array<double, 3> GeodeticNormal(double LatDeg, double LonDeg)
    {
        const double Lr = LatDeg * M_PI / 180.0;
        const double Nr = LonDeg * M_PI / 180.0;
        return { std::cos(Lr) * std::cos(Nr),
                 std::cos(Lr) * std::sin(Nr),
                 std::sin(Lr) };
    }

    // East-North-Up basis vectors at a geodetic position
    struct ENUBasis { std::array<double, 3> East, North, Up; };

    inline ENUBasis ComputeENUBasis(double LatDeg, double LonDeg)
    {
        const double Lr = LatDeg * M_PI / 180.0;
        const double Nr = LonDeg * M_PI / 180.0;
        const double SL = std::sin(Lr), CL = std::cos(Lr);
        const double SN = std::sin(Nr), CN = std::cos(Nr);
        return {
            { -SN,        CN,       0.0 },
            { -SL * CN,  -SL * SN,  CL  },
            {  CL * CN,   CL * SN,  SL  }
        };
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Quadtree tile identifier
    //
    //  Level 0 : 1 root tile [-180,180] × [-90,90]
    //  Level L : 2^L × 2^L tiles, each covering 360/2^L × 180/2^L degrees.
    //
    //  Tile (L, X, Y):
    //    LonMin = -180 + X * 360/2^L
    //    LatMin =  -90 + Y * 180/2^L
    // ─────────────────────────────────────────────────────────────────────────

    struct TileId
    {
        int Level{ 0 };
        int X{ 0 };
        int Y{ 0 };

        bool IsValid() const noexcept { return Level >= 0 && X >= 0 && Y >= 0; }
        static TileId Invalid() noexcept { return { -1, -1, -1 }; }

        bool operator==(const TileId& Other) const noexcept
        { return Level == Other.Level && X == Other.X && Y == Other.Y; }
        bool operator!=(const TileId& Other) const noexcept { return !(*this == Other); }
    };

    inline double TileWidthDeg(int Level)  noexcept { return 360.0 / (1 << Level); }
    inline double TileHeightDeg(int Level) noexcept { return 180.0 / (1 << Level); }

    inline TileId TileAtLatLon(double LatDeg, double LonDeg, int Level) noexcept
    {
        const int N = 1 << Level;
        int Tx = static_cast<int>((LonDeg + 180.0) / 360.0 * N);
        int Ty = static_cast<int>((LatDeg +  90.0) / 180.0 * N);
        if (Tx < 0) Tx = 0; else if (Tx >= N) Tx = N - 1;
        if (Ty < 0) Ty = 0; else if (Ty >= N) Ty = N - 1;
        return { Level, Tx, Ty };
    }

    // Child quadrant indices: 0=SW, 1=SE, 2=NW, 3=NE
    inline TileId ChildTile(const TileId& Parent, int Quadrant) noexcept
    {
        return { Parent.Level + 1,
                 Parent.X * 2 + (Quadrant & 1),
                 Parent.Y * 2 + ((Quadrant >> 1) & 1) };
    }

    inline TileId ParentTile(const TileId& Child) noexcept
    {
        if (!Child.IsValid() || Child.Level == 0) return TileId::Invalid();
        return { Child.Level - 1, Child.X / 2, Child.Y / 2 };
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Geographic bounding box
    // ─────────────────────────────────────────────────────────────────────────

    struct GeoBounds
    {
        double MinLat{ -90.0 }, MaxLat{ 90.0 };
        double MinLon{ -180.0 }, MaxLon{ 180.0 };

        double LatCenter() const noexcept { return (MinLat + MaxLat) * 0.5; }
        double LonCenter() const noexcept { return (MinLon + MaxLon) * 0.5; }
        double LatSpan()   const noexcept { return MaxLat - MinLat; }
        double LonSpan()   const noexcept { return MaxLon - MinLon; }
        bool Contains(double Lat, double Lon) const noexcept
        { return Lat >= MinLat && Lat <= MaxLat && Lon >= MinLon && Lon <= MaxLon; }
    };

    inline GeoBounds BoundsForTile(const TileId& Id) noexcept
    {
        const double W = TileWidthDeg(Id.Level);
        const double H = TileHeightDeg(Id.Level);
        return { -90.0  + Id.Y * H,       -90.0  + (Id.Y + 1) * H,
                 -180.0 + Id.X * W,       -180.0 + (Id.X + 1) * W };
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Terrain vertex (stored in .tbin files)
    //  Position is ECEF relative to the tile's CenterEcef (float precision).
    // ─────────────────────────────────────────────────────────────────────────

    struct TerrainVertex
    {
        float Px, Py, Pz;  // ECEF position − tile center (metres)
        float Nx, Ny, Nz;  // Surface normal (unit, ECEF)
        float U, V;         // Texture coordinates: U=0→W edge, V=0→S edge
    };

    // ─────────────────────────────────────────────────────────────────────────
    //  Binary tile file format  (.tbin)
    //
    //  Layout:
    //    TerrainTileHeader
    //    TerrainVertex[VertexCount]
    //    uint32_t[IndexCount]           (triangle list)
    //    uint8_t[TextureDataSize]       (JPEG bytes)
    // ─────────────────────────────────────────────────────────────────────────

    inline constexpr uint32_t TileMagic   = 0x4E524743u; // "CGRN"
    inline constexpr uint32_t TileVersion = 1u;

    struct TerrainTileHeader
    {
        uint32_t Magic;
        uint32_t Version;
        int32_t  TileLevel, TileX, TileY;
        double   MinLat, MaxLat, MinLon, MaxLon;
        double   CenterEcefX, CenterEcefY, CenterEcefZ;
        uint32_t VertexCount;
        uint32_t IndexCount;
        uint32_t TextureDataSize; // bytes of JPEG that follow the index data
    };

    // ─────────────────────────────────────────────────────────────────────────
    //  Quadtree index file  (.tidx)
    //
    //  Layout:
    //    TerrainIndexHeader
    //    TerrainIndexNode[NodeCount]
    // ─────────────────────────────────────────────────────────────────────────

    inline constexpr uint32_t IndexMagic   = 0x58444943u; // "CIDX"
    inline constexpr uint32_t IndexVersion = 1u;

    struct TerrainIndexHeader
    {
        uint32_t Magic;
        uint32_t Version;
        int32_t  MinLevel;
        int32_t  MaxLevel;
        uint32_t NodeCount;
    };

    struct TerrainIndexNode
    {
        int32_t TileLevel, TileX, TileY;
        double  MinLat, MaxLat, MinLon, MaxLon;
        double  CenterEcefX, CenterEcefY, CenterEcefZ;
        // Geometric error in metres: max ECEF displacement from true terrain
        float   GeometricError;
        uint8_t HasData;       // 1 if .tbin exists for this tile
        uint8_t ChildMask;     // bit i set → child quadrant i exists
        uint8_t Padding[2];
        int32_t ChildIndices[4]; // indices into node array, -1 = absent
    };

} // namespace Canavar::Globe

// ─── std::hash support for TileId (needed for unordered containers) ──────────
namespace std
{
    template <>
    struct hash<Canavar::Globe::TileId>
    {
        std::size_t operator()(const Canavar::Globe::TileId& Id) const noexcept
        {
            const std::size_t H1 = std::hash<int>()(Id.Level);
            const std::size_t H2 = std::hash<int>()(Id.X);
            const std::size_t H3 = std::hash<int>()(Id.Y);
            return H1 ^ (H2 * 2654435761u) ^ (H3 * 2246822519u);
        }
    };
}
