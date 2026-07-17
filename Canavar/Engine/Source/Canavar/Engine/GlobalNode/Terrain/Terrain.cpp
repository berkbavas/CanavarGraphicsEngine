#include "Terrain.h"

#include "Canavar/Engine/Manager/LightManager.h"
#include "Canavar/Engine/Manager/Renderer.h"

Canavar::Engine::Terrain::Terrain(Renderer *pRenderer)
    : mRenderer(pRenderer)
{
    initializeOpenGLFunctions();
    SetNodeName("Terrain");
    Generate();
    SetUpBuffers();
    SetUpTextures();
    SetUpShader();
}

Canavar::Engine::Terrain::~Terrain()
{
    // Clean up OpenGL resources
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mEBO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mPBO);
    glDeleteTextures(1, &mAlbedoTexture);
    glDeleteTextures(1, &mNormalTexture);
    glDeleteTextures(1, &mDisplacementTexture);
}

void Canavar::Engine::Terrain::Generate()
{
    auto VerticesXCount = (mSubdivision + 2);
    auto VerticesCount = VerticesXCount * VerticesXCount;
    auto IndicesCount = (VerticesXCount - 1) * (VerticesXCount - 1) * 6;
    auto StepSize = mWidth / (mSubdivision + 1);

    // Height map
    for (int z = 0; z < VerticesXCount; z++)
    {
        for (int x = 0; x < VerticesXCount; x++)
        {
            mPositions << QVector3D(x * StepSize, 0, z * StepSize);
        }
    }

    // Build Triangles
    for (size_t z = 0; z < VerticesXCount - 1; z++)
    {
        for (size_t x = 0; x < VerticesXCount - 1; x++)
        {
            auto Start = z * VerticesXCount + x;

            auto TopLeft = Start;
            auto TopRight = Start + 1;
            auto BotLeft = Start + VerticesXCount;
            auto BotRight = Start + VerticesXCount + 1;

            mIndices << TopLeft;
            mIndices << BotLeft;
            mIndices << BotRight;
            mIndices << TopRight;
            mIndices << TopLeft;
            mIndices << BotRight;
        }
    }

    // Tiles
    const QVector2D I = QVector2D(1, 0) * mWidth;
    const QVector2D J = QVector2D(0, 1) * mWidth;

    mTilePositions.resize(mTiles * mTiles);

    for (int i = 0; i < mTiles; i++)
    {
        for (int j = 0; j < mTiles; j++)
        {
            mTilePositions[j + i * mTiles] = (float) (j - mTiles / 2) * I + (float) (i - mTiles / 2) * J;
        }
    }
}

void Canavar::Engine::Terrain::SetUpBuffers()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Indices
    glGenBuffers(1, &mEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.constData(), GL_STATIC_DRAW);

    // Positions
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mPositions.size() * sizeof(QVector3D), mPositions.constData(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), (void *) 0);
    glEnableVertexAttribArray(0);

    // Tile Positions
    glGenBuffers(1, &mPBO);
    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferData(GL_ARRAY_BUFFER, mTilePositions.size() * sizeof(QVector2D), mTilePositions.constData(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glPatchParameteri(GL_PATCH_VERTICES, 3);

    glBindVertexArray(0);
}

void Canavar::Engine::Terrain::SetUpTextures()
{
    mAlbedoTexture = Load2DArray(
        {
            ":/Resources/Terrain/Beach/Albedo.jpg",
            ":/Resources/Terrain/Grass/Albedo.jpg",
            ":/Resources/Terrain/Rock/Albedo.jpg",
            ":/Resources/Terrain/Snow/Albedo.jpg",
        },
        5);

    mNormalTexture = Load2DArray(
        {
            ":/Resources/Terrain/Beach/Normal.jpg",
            ":/Resources/Terrain/Grass/Normal.jpg",
            ":/Resources/Terrain/Rock/Normal.jpg",
            ":/Resources/Terrain/Snow/Normal.jpg",
        },
        5);

    mDisplacementTexture = Load2DArray(
        {
            ":/Resources/Terrain/Beach/Displacement.jpg",
            ":/Resources/Terrain/Grass/Displacement.jpg",
            ":/Resources/Terrain/Rock/Displacement.jpg",
            ":/Resources/Terrain/Snow/Displacement.jpg",
        },
        5);
}

void Canavar::Engine::Terrain::SetUpShader()
{
    mTerrainShader = std::make_unique<Shader>("Terrain Shader");
    mTerrainShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Terrain.vert");
    mTerrainShader->AddPath(QOpenGLShader::TessellationControl, ":/Resources/Shaders/Terrain.tesc");
    mTerrainShader->AddPath(QOpenGLShader::TessellationEvaluation, ":/Resources/Shaders/Terrain.tese");
    mTerrainShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Terrain.frag");
    mTerrainShader->Initialize();
}

GLuint Canavar::Engine::Terrain::Load2DArray(const std::array<std::string, 4> &Filepaths, int Miplevels)
{
    std::array<int, 4> Widths;
    std::array<int, 4> Heights;
    std::array<QImage, 4> Images;

    for (int i = 0; i < Filepaths.size(); i++)
    {
        auto Filepath = Filepaths[i];

        QImage Image(QString::fromStdString(Filepath));
        if (Image.isNull())
        {
            CGE_EXIT_FAILURE("Terrain::Load2DArray: Failed to load image from '{}'", Filepath);
        }
        // Ensure the image is in RGBA format
        // Convert to RGBA8888 format for OpenGL compatibility
        // This is necessary because OpenGL expects images in RGBA format (4 byte per pixel)
        Images[i] = Image.convertToFormat(QImage::Format_RGBA8888);

        int Width = Image.width();
        int Height = Image.height();
        const uint8_t *pData = Image.constBits();

        Widths[i] = Width;
        Heights[i] = Height;
    }

    GLuint Id;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &Id);
    glTextureParameteri(Id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(Id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(Id, GL_TEXTURE_MAX_ANISOTROPY, 16);

    glTextureStorage3D(Id, Miplevels, GL_RGBA8, Widths[0], Heights[0], Filepaths.size());

    for (int i = 0; i < Filepaths.size(); i++)
    {
        auto w = Widths[i];
        auto h = Heights[i];
        glTextureSubImage3D(Id, 0, 0, 0, i, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, Images[i].constBits());
    }

    glGenerateTextureMipmap(Id);

    return Id;
}

QVector2D Canavar::Engine::Terrain::WhichTile(const QVector3D &Subject) const
{
    const int I = int(Subject.x()) / mWidth;
    const int J = int(Subject.z()) / mWidth;

    return QVector2D(I * mWidth, J * mWidth);
}

void Canavar::Engine::Terrain::TranslateTiles(const QVector2D &Translation)
{
    for (int i = 0; i < mTilePositions.size(); ++i)
    {
        mTilePositions[i] += Translation;
    }

    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mTilePositions.size() * sizeof(QVector2D), mTilePositions.constData());
}

void Canavar::Engine::Terrain::CalculateTilePositions(PerspectiveCamera *pCamera)
{
    const auto CurrentTilePosition = WhichTile(pCamera->GetPosition());

    if (CurrentTilePosition != mPreviousTilePosition)
    {
        TranslateTiles(CurrentTilePosition - mPreviousTilePosition);
        mPreviousTilePosition = CurrentTilePosition;
    }
}

void Canavar::Engine::Terrain::Render()
{
    if (mEnabled == false)
    {
        return;
    }

    const auto pCamera = mRenderer->GetActiveCamera();
    const auto pLightManager = mRenderer->GetLightManager();
    const auto pHaze = mRenderer->GetHaze();

    // Update tile positions based on the camera's current position
    CalculateTilePositions(pCamera);

    // Set light uniforms for the terrain shader
    pLightManager->SetDirectionalLightsUniforms(mTerrainShader.get());
    pLightManager->SetPointLightsUniforms(mTerrainShader.get(), pCamera->GetPosition(), pCamera->GetZFar());
    pHaze->SetUniforms(mTerrainShader.get());

    mTerrainShader->Bind();

    glBindTextureUnit(0, mAlbedoTexture);
    glBindTextureUnit(1, mNormalTexture);
    glBindTextureUnit(2, mDisplacementTexture);

    mTerrainShader->SetUniform("uViewProjectionMatrix", pCamera->GetViewProjectionMatrix());
    mTerrainShader->SetUniform("uFarPlane", pCamera->GetZFar());
    mTerrainShader->SetUniform("uCameraPosition", pCamera->GetPosition());

    mTerrainShader->SetUniform("uNodeId", GetNodeId());
    mTerrainShader->SetUniform("uTessellationMultiplier", mTessellationMultiplier);
    mTerrainShader->SetUniform("uWidth", mWidth);

    mTerrainShader->SetUniform("uNoise.Octaves", mOctaves);
    mTerrainShader->SetUniform("uNoise.Amplitude", mAmplitude);
    mTerrainShader->SetUniform("uNoise.Frequency", mFrequency);
    mTerrainShader->SetUniform("uNoise.Persistence", mPersistence);
    mTerrainShader->SetUniform("uNoise.Lacunarity", mLacunarity);

    mTerrainShader->SetUniform("uTerrain.Ambient", mAmbient);
    mTerrainShader->SetUniform("uTerrain.Diffuse", mDiffuse);
    mTerrainShader->SetUniform("uTerrain.Specular", mSpecular);
    mTerrainShader->SetUniform("uTerrain.Shininess", mShininess);

    mTerrainShader->SetUniformArray("uTextureStartHeights", mTextureStartHeights.data(), mTextureStartHeights.size(), 1);
    mTerrainShader->SetUniformArray("uTextureBlends", mTextureBlends.data(), mTextureBlends.size(), 1);
    mTerrainShader->SetUniformArray("uTextureSizes", mTextureSizes.data(), mTextureSizes.size(), 1);
    mTerrainShader->SetUniformArray("uTextureDisplacementWeights", mTextureDisplacementWeights.data(), mTextureDisplacementWeights.size(), 1);

    glBindVertexArray(mVAO);
    glDrawElementsInstanced(GL_PATCHES, mIndices.size(), GL_UNSIGNED_INT, 0, mTilePositions.size());
    glBindVertexArray(0);
    
    mTerrainShader->Unbind();
}
