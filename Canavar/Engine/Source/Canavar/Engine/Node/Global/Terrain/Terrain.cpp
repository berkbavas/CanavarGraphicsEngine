#include "Terrain.h"

Canavar::Engine::Terrain::Terrain()
{
    initializeOpenGLFunctions();
    SetNodeName("Terrain");
    Generate();
    SetUpBuffers();
    SetUpTextures();
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
            ":/Resources/Terrain/beach/albedo.jpg",
            ":/Resources/Terrain/grass/albedo.jpg",
            ":/Resources/Terrain/rock/albedo.jpg",
            ":/Resources/Terrain/snow/albedo.jpg",
        },
        5);

    mNormalTexture = Load2DArray(
        {
            ":/Resources/Terrain/beach/normal.jpg",
            ":/Resources/Terrain/grass/normal.jpg",
            ":/Resources/Terrain/rock/normal.jpg",
            ":/Resources/Terrain/snow/normal.jpg",
        },
        5);

    mDisplacementTexture = Load2DArray(
        {
            ":/Resources/Terrain/beach/displacement.jpg",
            ":/Resources/Terrain/grass/displacement.jpg",
            ":/Resources/Terrain/rock/displacement.jpg",
            ":/Resources/Terrain/snow/displacement.jpg",
        },
        5);
}

void Canavar::Engine::Terrain::CleanUp()
{
    // TODO: Implement CleanUp method to release OpenGL resources
}

GLuint Canavar::Engine::Terrain::Load2DArray(const std::array<std::string, 4> &filepaths, int miplevels)
{
    std::array<int, 4> widths;
    std::array<int, 4> heights;
    std::array<QImage, 4> images;

    for (int i = 0; i < filepaths.size(); i++)
    {
        auto filepath = filepaths[i];

        QImage image(QString::fromStdString(filepath));
        if (image.isNull())
        {
            CGE_EXIT_FAILURE("Terrain::Load2DArray: Failed to load image from '{}'", filepath);
        }
        // Ensure the image is in RGBA format
        // Convert to RGBA8888 format for OpenGL compatibility
        // This is necessary because OpenGL expects images in RGBA format (4 byte per pixel)
        images[i] = image.convertToFormat(QImage::Format_RGBA8888);

        int width = image.width();
        int height = image.height();
        const uint8_t *data = image.constBits();

        widths[i] = width;
        heights[i] = height;
    }

    GLuint gl_id;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &gl_id);
    glTextureParameteri(gl_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(gl_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(gl_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(gl_id, GL_TEXTURE_MAX_ANISOTROPY, 16);

    glTextureStorage3D(gl_id, miplevels, GL_RGBA8, widths[0], heights[0], filepaths.size());

    for (int i = 0; i < filepaths.size(); i++)
    {
        auto w = widths[i];
        auto h = heights[i];
        glTextureSubImage3D(gl_id, 0, 0, 0, i, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[i].constBits());
    }

    glGenerateTextureMipmap(gl_id);

    return gl_id;
}

QVector2D Canavar::Engine::Terrain::WhichTile(const QVector3D &subject) const
{
    int i = int(subject.x()) / mWidth;
    int j = int(subject.z()) / mWidth;

    return QVector2D(i * mWidth, j * mWidth);
}

void Canavar::Engine::Terrain::TranslateTiles(const QVector2D &translation)
{
    for (int i = 0; i < mTilePositions.size(); ++i)
    {
        mTilePositions[i] += translation;
    }

    glBindBuffer(GL_ARRAY_BUFFER, mPBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mTilePositions.size() * sizeof(QVector2D), mTilePositions.constData());
}

void Canavar::Engine::Terrain::Render(Shader *pShader, Camera *pCamera)
{
    if (mEnabled == false)
    {
        return;
    }

    QVector2D currentTilePosition = WhichTile(pCamera->GetWorldPosition());

    if (currentTilePosition != mPreviousTilePosition)
    {
        TranslateTiles(currentTilePosition - mPreviousTilePosition);
        mPreviousTilePosition = currentTilePosition;

        qDebug() << "Terrain::Render:" << "currentTilePosition:" << currentTilePosition;
    }

    pShader->Bind();
    glBindTextureUnit(0, mAlbedoTexture);
    glBindTextureUnit(1, mNormalTexture);
    glBindTextureUnit(2, mDisplacementTexture);

    pShader->SetUniformValue("node_id", static_cast<float>(GetNodeId()));
    pShader->SetUniformValue("tess_multiplier", mTesselationMultiplier);
    pShader->SetUniformValue("width", mWidth);

    pShader->SetUniformValue("noise.num_octaves", mOctaves);
    pShader->SetUniformValue("noise.amplitude", mAmplitude);
    pShader->SetUniformValue("noise.frequency", mFrequency);
    pShader->SetUniformValue("noise.persistence", mPersistence);
    pShader->SetUniformValue("noise.lacunarity", mLacunarity);

    pShader->SetUniformValue("terrain.ambient", mAmbient);
    pShader->SetUniformValue("terrain.diffuse", mDiffuse);
    pShader->SetUniformValue("terrain.specular", mSpecular);
    pShader->SetUniformValue("terrain.shininess", mShininess);

    pShader->SetUniformValueArray("texture_start_heights", mTextureStartHeights.data(), mTextureStartHeights.size(), 1);
    pShader->SetUniformValueArray("texture_blends", mTextureBlends.data(), mTextureBlends.size(), 1);
    pShader->SetUniformValueArray("texture_sizes", mTextureSizes.data(), mTextureSizes.size(), 1);
    pShader->SetUniformValueArray("texture_displacement_weights", mTextureDisplacementWeights.data(), mTextureDisplacementWeights.size(), 1);

    glBindVertexArray(mVAO);

    glDrawElementsInstanced(GL_PATCHES, mIndices.size(), GL_UNSIGNED_INT, 0, mTilePositions.size());

    glBindVertexArray(0);
    pShader->Release();
}
