#include "MeshBuilder.h"
#include "ElevationSampler.h"

#include <cmath>

namespace Canavar::Globe
{
    MeshBuilder::MeshBuilder(int GridRes)
        : mGridRes(GridRes)
    {}

    bool MeshBuilder::Build(const GeoBounds&         Bounds,
                             ElevationSampler&         Sampler,
                             QVector<TerrainVertex>&   VerticesOut,
                             QVector<uint32_t>&        IndicesOut,
                             std::array<double, 3>&    CenterEcefOut)
    {
        const int N = mGridRes;         // vertices per side
        const int Q = N - 1;            // quads per side

        // ── tile centre ECEF (at surface, no elevation) ───────────────────────
        CenterEcefOut = GeodeticToECEF(Bounds.LatCenter(), Bounds.LonCenter(), 0.0);

        // ── build vertex grid ─────────────────────────────────────────────────
        VerticesOut.resize(N * N);

        // We sample elevation at a slightly denser grid for normal estimation.
        // Step in degrees for neighbour offset used in normal computation.
        const double LatStep = Bounds.LatSpan() / Q;
        const double LonStep = Bounds.LonSpan() / Q;
        const double Eps     = LatStep * 0.25; // small offset for finite differences

        for (int Row = 0; Row < N; ++Row)
        {
            for (int Col = 0; Col < N; ++Col)
            {
                // Normalised texture coordinates: U = west→east, V = south→north
                const float U = static_cast<float>(Col) / Q;
                const float V = static_cast<float>(Row) / Q;

                const double Lon = Bounds.MinLon + Col * LonStep;
                const double Lat = Bounds.MinLat + Row * LatStep;

                const double Elev    = Sampler.SampleElevation(Lat, Lon);
                const auto   PosECEF = GeodeticToECEF(Lat, Lon, Elev);

                // Normal via central-difference in ECEF space
                const double ElevE = Sampler.SampleElevation(Lat,       Lon + Eps);
                const double ElevW = Sampler.SampleElevation(Lat,       Lon - Eps);
                const double ElevN = Sampler.SampleElevation(Lat + Eps, Lon      );
                const double ElevS = Sampler.SampleElevation(Lat - Eps, Lon      );

                const auto PE = GeodeticToECEF(Lat,       Lon + Eps, ElevE);
                const auto PW = GeodeticToECEF(Lat,       Lon - Eps, ElevW);
                const auto PN = GeodeticToECEF(Lat + Eps, Lon,       ElevN);
                const auto PS = GeodeticToECEF(Lat - Eps, Lon,       ElevS);

                // Tangent vectors
                const double Tx = PE[0] - PW[0], Ty = PE[1] - PW[1], Tz = PE[2] - PW[2];
                const double Bx = PN[0] - PS[0], By = PN[1] - PS[1], Bz = PN[2] - PS[2];

                // Cross product T × B
                const double Nx = Ty * Bz - Tz * By;
                const double Ny = Tz * Bx - Tx * Bz;
                const double Nz = Tx * By - Ty * Bx;
                const double NLen = std::sqrt(Nx * Nx + Ny * Ny + Nz * Nz);

                const float InvLen = (NLen > 1e-12) ? static_cast<float>(1.0 / NLen) : 1.0f;

                TerrainVertex& Vtx = VerticesOut[Row * N + Col];
                Vtx.Px = static_cast<float>(PosECEF[0] - CenterEcefOut[0]);
                Vtx.Py = static_cast<float>(PosECEF[1] - CenterEcefOut[1]);
                Vtx.Pz = static_cast<float>(PosECEF[2] - CenterEcefOut[2]);
                Vtx.Nx = static_cast<float>(Nx) * InvLen;
                Vtx.Ny = static_cast<float>(Ny) * InvLen;
                Vtx.Nz = static_cast<float>(Nz) * InvLen;
                Vtx.U  = U;
                Vtx.V  = V;
            }
        }

        // ── build index list (CCW triangles) ──────────────────────────────────
        IndicesOut.resize(Q * Q * 6);
        int Idx = 0;
        for (int Row = 0; Row < Q; ++Row)
        {
            for (int Col = 0; Col < Q; ++Col)
            {
                const uint32_t BL = static_cast<uint32_t>(Row       * N + Col);
                const uint32_t BR = static_cast<uint32_t>(Row       * N + Col + 1);
                const uint32_t TL = static_cast<uint32_t>((Row + 1) * N + Col);
                const uint32_t TR = static_cast<uint32_t>((Row + 1) * N + Col + 1);

                IndicesOut[Idx++] = BL;
                IndicesOut[Idx++] = BR;
                IndicesOut[Idx++] = TR;
                IndicesOut[Idx++] = BL;
                IndicesOut[Idx++] = TR;
                IndicesOut[Idx++] = TL;
            }
        }

        return true;
    }

} // namespace Canavar::Globe
