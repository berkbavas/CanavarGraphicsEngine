#pragma once

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"

#include <QOpenGLFunctions_4_5_Core>
#include <QVector2D>

namespace Canavar::Engine
{
    class NodeManager;
    class ShaderManager;

    struct Character
    {
        unsigned int TextureID; // ID handle of the glyph texture
        QVector2D Size;         // Size of glyph
        QVector2D Bearing;      // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Offset to advance to next glyph
    };

    class TextRenderer : protected QOpenGLFunctions_4_5_Core
    {
      public:
        TextRenderer() = default;
        ~TextRenderer();

        void Initialize();

        void Render(Camera *pActiveCamera);

        void Resize(int w, int h);

      private:
        void Render2DTexts();
        void Render3DTexts();

        void RenderText(Shader *pShader, const QString &text, float x, float y, float scale, const QVector3D &color);
        void RenderCharacter(Shader *pShader, const Character &ch, float x, float y, float scale);

        void InitializeCharacters();
        void InitializeBuffers();

        std::map<QChar, Character> mCharacters; // Holds all loaded characters

        Shader *mText2DShader{ nullptr };
        Shader *mText3DShader{ nullptr };

        Camera *mActiveCamera{ nullptr };

        QMatrix4x4 mProjectionMatrix;

        // Vertex Array Object and Vertex Buffer Object for text rendering
        GLuint mVAO{ 0 };
        GLuint mVBO{ 0 };

        DEFINE_MEMBER_PTR(NodeManager, NodeManager);
        DEFINE_MEMBER_PTR(ShaderManager, ShaderManager);

        static constexpr const char *FONT_PATH = ":/Resources/Font/OpenSans/OpenSans-VariableFont_wdth,wght.ttf";
    };

    using TextRendererPtr = std::shared_ptr<TextRenderer>;
}