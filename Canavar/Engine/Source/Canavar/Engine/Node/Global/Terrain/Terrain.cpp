#include "Terrain.h"

#include "Canavar/Engine/Node/NodeVisitor.h"

Canavar::Engine::Terrain::Terrain()
{
    initializeOpenGLFunctions();
    SetNodeName("Terrain");
    Generate();
    SetUpBuffers();
    SetUpTextures();
}

void Canavar::Engine::Terrain::ToJson(QJsonObject &Object)
{
    Global::ToJson(Object);

    Object["octaves"] = mOctaves;
    Object["amplitude"] = mAmplitude;
    Object["frequency"] = mFrequency;
    Object["persistence"] = mPersistence;
    Object["lacunarity"] = mLacunarity;
    Object["tessellation_multiplier"] = mTessellationMultiplier;

    Object["ambient"] = mAmbient;
    Object["diffuse"] = mDiffuse;
    Object["specular"] = mSpecular;
    Object["shininess"] = mShininess;
    Object["enabled"] = mEnabled;
}

void Canavar::Engine::Terrain::FromJson(const QJsonObject &Object, const QSet<NodePtr> &Nodes)
{
    Global::FromJson(Object, Nodes);

    mOctaves = Object["octaves"].toInt(7);
    mAmplitude = Object["amplitude"].toDouble(1055.0f);
    mFrequency = Object["frequency"].toDouble(0.110f);
    mPersistence = Object["persistence"].toDouble(0.063f);
    mLacunarity = Object["lacunarity"].toDouble(8.150f);
    mTessellationMultiplier = Object["tessellation_multiplier"].toDouble(8);

    mAmbient = Object["ambient"].toDouble(0.25f);
    mDiffuse = Object["diffuse"].toDouble(0.75f);
    mSpecular = Object["specular"].toDouble(0.25f);
    mShininess = Object["shininess"].toDouble(8.0f);
    mEnabled = Object["enabled"].toBool(true);
}

void Canavar::Engine::Terrain::Generate()
{
    auto vertices_x_count = (mSubdivision + 2);
    auto vertices_count = vertices_x_count * vertices_x_count;
    auto indices_count = (vertices_x_count - 1) * (vertices_x_count - 1) * 6;
    auto step_size = mWidth / (mSubdivision + 1);

    // Height map
    for (int z = 0; z < vertices_x_count; z++)
    {
        for (int x = 0; x < vertices_x_count; x++)
        {
            mPositions << QVector3D(x * step_size, 0, z * step_size);
        }
    }

    // Build Triangles
    for (size_t z = 0; z < vertices_x_count - 1; z++)
    {
        for (size_t x = 0; x < vertices_x_count - 1; x++)
        {
            auto start = z * vertices_x_count + x;

            auto top_left = start;
            auto top_right = start + 1;
            auto bot_left = start + vertices_x_count;
            auto bot_right = start + vertices_x_count + 1;

            mIndices << top_left;
            mIndices << bot_left;
            mIndices << bot_right;
            mIndices << top_right;
            mIndices << top_left;
            mIndices << bot_right;
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

void Canavar::Engine::Terrain::CleanUp()
{
    // TODO: Implement CleanUp method to release OpenGL resources
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

        int width = Image.width();
        int height = Image.height();
        const uint8_t *data = Image.constBits();

        Widths[i] = width;
        Heights[i] = height;
    }

    GLuint gl_id;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &gl_id);
    glTextureParameteri(gl_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(gl_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(gl_id, GL_TEXTURE_MAX_ANISOTROPY, 16);

    glTextureStorage3D(gl_id, Miplevels, GL_RGBA8, Widths[0], Heights[0], Filepaths.size());

    for (int i = 0; i < Filepaths.size(); i++)
    {
        auto w = Widths[i];
        auto h = Heights[i];
        glTextureSubImage3D(gl_id, 0, 0, 0, i, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, Images[i].constBits());
    }

    glGenerateTextureMipmap(gl_id);

    return gl_id;
}

QVector2D Canavar::Engine::Terrain::WhichTile(const QVector3D &Subject) const
{
    int i = int(Subject.x()) / mWidth;
    int j = int(Subject.z()) / mWidth;

    return QVector2D(i * mWidth, j * mWidth);
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

void Canavar::Engine::Terrain::Accept(NodeVisitor &Visitor)
{
    Visitor.Visit(*this);
}

void Canavar::Engine::Terrain::Render(Shader *pShader, Camera *pCamera)
{
    if (mEnabled == false)
    {
        return;
    }

    QVector2D CurrentTilePosition = WhichTile(pCamera->GetWorldPosition());

    if (CurrentTilePosition != mPreviousTilePosition)
    {
        TranslateTiles(CurrentTilePosition - mPreviousTilePosition);
        mPreviousTilePosition = CurrentTilePosition;

        qDebug() << "Terrain::Render:" << "CurrentTilePosition:" << CurrentTilePosition;
    }

    pShader->Bind();

    glBindTextureUnit(0, mAlbedoTexture);
    glBindTextureUnit(1, mNormalTexture);
    glBindTextureUnit(2, mDisplacementTexture);

    pShader->SetUniformValue("uNodeId", GetNodeId());
    pShader->SetUniformValue("uTessellationMultiplier", mTessellationMultiplier);
    pShader->SetUniformValue("uWidth", mWidth);

    pShader->SetUniformValue("uNoise.Octaves", mOctaves);
    pShader->SetUniformValue("uNoise.Amplitude", mAmplitude);
    pShader->SetUniformValue("uNoise.Frequency", mFrequency);
    pShader->SetUniformValue("uNoise.Persistence", mPersistence);
    pShader->SetUniformValue("uNoise.Lacunarity", mLacunarity);

    pShader->SetUniformValue("uTerrain.Ambient", mAmbient);
    pShader->SetUniformValue("uTerrain.Diffuse", mDiffuse);
    pShader->SetUniformValue("uTerrain.Specular", mSpecular);
    pShader->SetUniformValue("uTerrain.Shininess", mShininess);

    pShader->SetUniformValueArray("uTextureStartHeights", mTextureStartHeights.data(), mTextureStartHeights.size(), 1);
    pShader->SetUniformValueArray("uTextureBlends", mTextureBlends.data(), mTextureBlends.size(), 1);
    pShader->SetUniformValueArray("uTextureSizes", mTextureSizes.data(), mTextureSizes.size(), 1);
    pShader->SetUniformValueArray("uTextureDisplacementWeights", mTextureDisplacementWeights.data(), mTextureDisplacementWeights.size(), 1);

    glBindVertexArray(mVAO);

    glDrawElementsInstanced(GL_PATCHES, mIndices.size(), GL_UNSIGNED_INT, 0, mTilePositions.size());

    glBindVertexArray(0);
    pShader->Release();
}
