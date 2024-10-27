#include "Terrain.h"

#include "Canavar/Engine/Node/Camera/Camera.h"
#include "Canavar/Engine/Util/ModelImporter.h"
#include "Canavar/Engine/Util/Shader.h"

#include <QMatrix4x4>

void Canavar::Engine::Terrain::Initialize()
{
    initializeOpenGLFunctions();

    Reset();

    mTileGenerator = new TileGenerator(3, 128, 1024.0f);

    mTextures.insert("Sand", ModelImporter::CreateTexture(":/Resources/Terrain/sand.jpg"));
    mTextures.insert("Grass", ModelImporter::CreateTexture(":/Resources/Terrain/grass0.jpg"));
    mTextures.insert("Snow", ModelImporter::CreateTexture(":/Resources/Terrain/snow0.jpg"));
    mTextures.insert("RockDiffuse", ModelImporter::CreateTexture(":/Resources/Terrain/rock0.jpg"));
    mTextures.insert("RockNormal", ModelImporter::CreateTexture(":/Resources/Terrain/rnormal.jpg"));
    mTextures.insert("Terrain", ModelImporter::CreateTexture(":/Resources/Terrain/terrain.jpg"));

    SetScale(QVector3D(1, 0, 1));
}

void Canavar::Engine::Terrain::Render(Shader* pShader, Camera* pCamera)
{
    if (!mEnabled)
    {
        return;
    }

    QVector2D currentTilePosition = mTileGenerator->WhichTile(pCamera->GetWorldPosition());

    if (currentTilePosition != mPreviousTilePosition)
    {
        mTileGenerator->TranslateTiles((currentTilePosition - mPreviousTilePosition));
        mPreviousTilePosition = currentTilePosition;
    }

    pShader->Bind();
    pShader->SetUniformValue("M", GetWorldTransformation());
    pShader->SetUniformValue("terrain.amplitude", mAmplitude);
    pShader->SetUniformValue("terrain.seed", mSeed);
    pShader->SetUniformValue("terrain.octaves", mOctaves);
    pShader->SetUniformValue("terrain.frequency", mFrequency);
    pShader->SetUniformValue("terrain.tessellationMultiplier", mTessellationMultiplier);
    pShader->SetUniformValue("terrain.power", mPower);
    pShader->SetUniformValue("terrain.grassCoverage", mGrassCoverage);
    pShader->SetUniformValue("terrain.ambient", mAmbient);
    pShader->SetUniformValue("terrain.diffuse", mDiffuse);
    pShader->SetUniformValue("terrain.shininess", mShininess);
    pShader->SetUniformValue("terrain.specular", mSpecular);
    pShader->SetUniformValue("waterHeight", -1000.0f);
    pShader->SetSampler("sand", 1, mTextures.value("Sand")->textureId());
    pShader->SetSampler("grass", 2, mTextures.value("Grass")->textureId());
    pShader->SetSampler("terrainTexture", 3, mTextures.value("Terrain")->textureId());
    // pShader->SetSampler("snow", 4, mTextures.value("Snow")->textureId());
    pShader->SetSampler("rock", 5, mTextures.value("RockDiffuse")->textureId());
    pShader->SetSampler("rockNormal", 6, mTextures.value("RockNormal")->textureId());
    mTileGenerator->Render(GL_PATCHES);
    pShader->Release();
}

void Canavar::Engine::Terrain::Reset()
{
    mOctaves = 13;
    mFrequency = 0.01f;
    mTessellationMultiplier = 1.0f;
    mAmplitude = 20.0f;
    mPower = 3.0f;
    mGrassCoverage = 0.45f;
    mSeed = QVector3D(1, 1, 1);

    mAmbient = 0.5f;
    mDiffuse = 0.6f;
    mShininess = 8.0f;
    mSpecular = 0.05f;
}