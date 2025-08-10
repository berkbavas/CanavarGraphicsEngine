#include "TextRenderer.h"

#include "Canavar/Engine/Manager/NodeManager.h"
#include "Canavar/Engine/Manager/ShaderManager.h"
#include "Canavar/Engine/Node/Object/Text/Text2D.h"
#include "Canavar/Engine/Node/Object/Text/Text3D.h"
#include "Canavar/Engine/Util/Util.h"

#include <ft2build.h>

#include FT_FREETYPE_H

Canavar::Engine::TextRenderer::~TextRenderer()
{
    if (mVBO)
    {
        glDeleteBuffers(1, &mVBO);
        mVBO = 0;
    }

    if (mVAO)
    {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
    }

    if (mText2DShader)
    {
        delete mText2DShader;
        mText2DShader = nullptr;
    }

    if (mText3DShader)
    {
        delete mText3DShader;
        mText3DShader = nullptr;
    }

    for (const auto &[c, character] : mCharacters)
    {
        glDeleteTextures(1, &character.TextureID);
    }

    mCharacters.clear();
}

void Canavar::Engine::TextRenderer::Initialize()
{
    mText2DShader = mShaderManager->GetShader(ShaderType::Text2D);
    mText3DShader = mShaderManager->GetShader(ShaderType::Text3D);

    initializeOpenGLFunctions();
    InitializeBuffers();
    InitializeCharacters();
}

void Canavar::Engine::TextRenderer::InitializeCharacters()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        CGE_EXIT_FAILURE("TextRenderer::InitializeCharacters: Could not init FreeType Library.");
    }

    QByteArray bytes = Util::GetBytes(FONT_PATH);

    if (bytes.size() == 0)
    {
        CGE_EXIT_FAILURE("TextRenderer::InitializeCharacters: Font file '{}' could not be found.", FONT_PATH);
    }

    FT_Face face;

    if (FT_New_Memory_Face(ft, reinterpret_cast<FT_Byte *>(bytes.data()), bytes.size(), 0, &face))
    {
        CGE_EXIT_FAILURE("TextRenderer::InitializeCharacters: Failed to load font.");
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 32);

    // Initialize character textures and other OpenGL settings here
    // This is typically done by loading a font and generating textures for each character.
    // For example, you might use FreeType to load a font and create Character objects.
    // mCharacters['A'] = { textureID, size, bearing, advance };

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LOG_FATAL("TextRenderer::InitializeCharacters:  Failed to load Glyph for character '{}'.", c);
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = { texture,                                                        //
                                QVector2D(face->glyph->bitmap.width, face->glyph->bitmap.rows), //
                                QVector2D(face->glyph->bitmap_left, face->glyph->bitmap_top),   //
                                static_cast<unsigned int>(face->glyph->advance.x) };
        mCharacters.insert(std::pair<QChar, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void Canavar::Engine::TextRenderer::InitializeBuffers()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Canavar::Engine::TextRenderer::Resize(int w, int h)
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.ortho(0.0f, static_cast<float>(w), 0.0f, static_cast<float>(h), -1.0f, 1.0f);
}

void Canavar::Engine::TextRenderer::Render(Camera *pActiveCamera)
{
    mActiveCamera = pActiveCamera;

    Render3DTexts();

    glDisable(GL_DEPTH_TEST);
    Render2DTexts();
    glEnable(GL_DEPTH_TEST);
}

void Canavar::Engine::TextRenderer::Render2DTexts()
{
    // Enable blending for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind the text shader
    mText2DShader->Bind();
    mText2DShader->SetUniformValue("uProjection", mProjectionMatrix);

    // Bind the vertex array
    glBindVertexArray(mVAO);

    const auto &Nodes = mNodeManager->GetNodes();

    for (const auto &pNode : Nodes)
    {
        if (const auto pText2D = std::dynamic_pointer_cast<Text2D>(pNode))
        {
            RenderText(mText2DShader, pText2D->GetText(), pText2D->GetPosition().x(), pText2D->GetPosition().y(), pText2D->GetScale(), pText2D->GetColor());
        }
    }

    // Unbind the vertex array
    glBindVertexArray(0);

    // Unbind the text shader
    mText2DShader->Release();

    // Disable blending
    glDisable(GL_BLEND);
}

void Canavar::Engine::TextRenderer::Render3DTexts()
{
    // Enable blending for text rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind the text shader
    mText3DShader->Bind();
    mText3DShader->SetUniformValue("uZFar", dynamic_cast<PerspectiveCamera *>(mActiveCamera)->GetZFar());

    // Bind the vertex array
    glBindVertexArray(mVAO);

    const auto &Nodes = mNodeManager->GetNodes();

    for (const auto &pNode : Nodes)
    {
        if (const auto pText3D = std::dynamic_pointer_cast<Text3D>(pNode))
        {
            mText3DShader->SetUniformValue("uModelMatrix", pText3D->GetWorldTransformation());
            mText3DShader->SetUniformValue("uMVP", mActiveCamera->GetViewProjectionMatrix() * pText3D->GetWorldTransformation());
            mText3DShader->SetUniformValue("uNodeId",pText3D->GetNodeId());
            RenderText(mText3DShader, pText3D->GetText(), 0, 0, 1, pText3D->GetColor());
        }
    }

    // Unbind the vertex array
    glBindVertexArray(0);

    // Unbind the text shader
    mText3DShader->Release();

    // Disable blending
    glDisable(GL_BLEND);
}

void Canavar::Engine::TextRenderer::RenderText(Shader *pShader, const QString &text, float x, float y, float scale, const QVector3D &color)
{
    // Set the text color
    pShader->SetUniformValue("uTextColor", color);

    // Render each character
    for (const QChar &c : text)
    {
        const auto it = mCharacters.find(c);
        // If the character is not found, log an error and skip rendering
        if (it != mCharacters.end())
        {
            const auto &ch = it->second;
            RenderCharacter(pShader, ch, x, y, scale);
            x += (ch.Advance >> 6) * scale; // Advance cursor
        }
        else
        {
            LOG_WARN("TextRenderer::RenderCharacter: Character '{}' not found.", c.toLatin1());
        }
    }
}

void Canavar::Engine::TextRenderer::RenderCharacter(Shader *pShader, const Character &ch, float x, float y, float scale)
{
    float xpos = x + ch.Bearing.x() * scale;
    float ypos = y - (ch.Size.y() - ch.Bearing.y()) * scale;

    float w = ch.Size.x() * scale;
    float h = ch.Size.y() * scale;

    // Update VBO for each character
    float vertices[6][4] = { { xpos, ypos + h, 0.0f, 0.0f }, //
                             { xpos, ypos, 0.0f, 1.0f },     //
                             { xpos + w, ypos, 1.0f, 1.0f }, //
                             { xpos, ypos + h, 0.0f, 0.0f }, //
                             { xpos + w, ypos, 1.0f, 1.0f }, //
                             { xpos + w, ypos + h, 1.0f, 0.0f } };

    // Render glyph texture over quad
    pShader->SetSampler("uTextTexture", 0, ch.TextureID, GL_TEXTURE_2D);

    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
}