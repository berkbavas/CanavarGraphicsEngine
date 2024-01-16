#pragma once

#include <QObject>

#define DEFINE_MEMBER(type, name) \
protected: \
    type m##name; \
\
public: \
    inline const type &Get##name(void) const { return m##name; } \
\
public: \
    inline type &Get##name##_NonConst(void) { return m##name; } \
\
public: \
    inline void Set##name(const type &var) { m##name = var; }

#define DEFINE_MEMBER_CONST(type, name) \
protected: \
    type m##name; \
\
public: \
    inline const type &Get##name(void) const { return m##name; }

namespace Canavar {
    namespace Engine {
        class Model;

        enum class ShaderType { //
            None,
            ModelColoredShader,
            ModelTexturedShader,
            SkyShader,
            TerrainShader,
            BlurShader,
            PostProcessShader,
            ScreenShader,
            NodeInfoShader,
            NozzleEffectShader,
            FirecrackerEffectShader,
            BasicShader,
            MeshVertexRendererShader,
            VertexInfoShader,
            LineStripShader,
            RaycasterShader,
            LightningStrikeShader,
            LightningStrikeQuadGeneratorShader,
        };

        enum class RenderMode { //
            Default = 0x00,
            NodeInfo = 0x01,
            Custom = 0x02,
            Raycaster = 0x04
        };

        Q_DECLARE_FLAGS(RenderModes, RenderMode);

        extern const QVector3D CUBE[36];
        extern const QVector2D QUAD[12];
        extern const QVector3D CUBE_STRIP[17];
    } // namespace Engine
} // namespace Canavar

Q_DECLARE_OPERATORS_FOR_FLAGS(Canavar::Engine::RenderModes)