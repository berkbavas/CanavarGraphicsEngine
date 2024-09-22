#include "Terrain.h"
#include "CameraManager.h"
#include "ShaderManager.h"

#include "Helper.h"

#include <QMatrix4x4>

Canavar::Engine::Terrain::Terrain()
    : Node()
    , mEnabled(true)
{
    mType = Node::NodeType::Terrain;
    mName = "Terrain";
    mSelectable = false;

    initializeOpenGLFunctions();

    mShaderManager = ShaderManager::Instance();
    mCameraManager = CameraManager::Instance();

    Reset();

    mTileGenerator = new TileGenerator(3, 128, 1024.0f);

    mTextures.insert("Sand", Helper::CreateTexture("Resources/Terrain/sand.jpg"));
    mTextures.insert("Grass", Helper::CreateTexture("Resources/Terrain/grass0.jpg"));
    mTextures.insert("Snow", Helper::CreateTexture("Resources/Terrain/snow0.jpg"));
    mTextures.insert("RockDiffuse", Helper::CreateTexture("Resources/Terrain/rock0.jpg"));
    mTextures.insert("RockNormal", Helper::CreateTexture("Resources/Terrain/rnormal.jpg"));
    mTextures.insert("Terrain", Helper::CreateTexture("Resources/Terrain/terrain.jpg"));

    SetScale(QVector3D(1, 0, 1));
}

void Canavar::Engine::Terrain::Render()
{
    if (!mEnabled)
        return;

    QVector2D currentTilePosition = mTileGenerator->WhichTile(mCameraManager->GetActiveCamera()->WorldPosition());

    if (currentTilePosition != mPreviousTilePosition)
    {
        mTileGenerator->TranslateTiles((currentTilePosition - mPreviousTilePosition));
        mPreviousTilePosition = currentTilePosition;
    }

    mShaderManager->Bind(ShaderType::TerrainShader);
    mShaderManager->SetUniformValue("M", WorldTransformation());
    mShaderManager->SetUniformValue("terrain.amplitude", mAmplitude);
    mShaderManager->SetUniformValue("terrain.seed", mSeed);
    mShaderManager->SetUniformValue("terrain.octaves", mOctaves);
    mShaderManager->SetUniformValue("terrain.frequency", mFrequency);
    mShaderManager->SetUniformValue("terrain.tessellationMultiplier", mTessellationMultiplier);
    mShaderManager->SetUniformValue("terrain.power", mPower);
    mShaderManager->SetUniformValue("terrain.grassCoverage", mGrassCoverage);
    mShaderManager->SetUniformValue("terrain.ambient", mAmbient);
    mShaderManager->SetUniformValue("terrain.diffuse", mDiffuse);
    mShaderManager->SetUniformValue("terrain.shininess", mShininess);
    mShaderManager->SetUniformValue("terrain.specular", mSpecular);
    mShaderManager->SetUniformValue("waterHeight", -1000.0f);
    mShaderManager->SetSampler("sand", 1, mTextures.value("Sand")->textureId());
    mShaderManager->SetSampler("grass", 2, mTextures.value("Grass")->textureId());
    mShaderManager->SetSampler("terrainTexture", 3, mTextures.value("Terrain")->textureId());
    mShaderManager->SetSampler("snow", 4, mTextures.value("Snow")->textureId());
    mShaderManager->SetSampler("rock", 5, mTextures.value("RockDiffuse")->textureId());
    mShaderManager->SetSampler("rockNormal", 6, mTextures.value("RockNormal")->textureId());

    mTileGenerator->Render(GL_PATCHES);
    mShaderManager->Release();
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

void Canavar::Engine::Terrain::ToJson(QJsonObject& object)
{
    Node::ToJson(object);

    object.insert("amplitude", mAmplitude);
    object.insert("frequency", mFrequency);
    object.insert("octaves", mOctaves);
    object.insert("power", mPower);
    object.insert("tessellation_multiplier", mTessellationMultiplier);
    object.insert("grass_coverage", mGrassCoverage);
    object.insert("ambient", mAmbient);
    object.insert("diffuse", mDiffuse);
    object.insert("specular", mSpecular);
    object.insert("shininess", mShininess);
    object.insert("enabled", mEnabled);

    QJsonObject seed;
    seed.insert("x", mSeed.x());
    seed.insert("y", mSeed.y());
    seed.insert("z", mSeed.z());

    object.insert("seed", seed);
}

void Canavar::Engine::Terrain::FromJson(const QJsonObject& object)
{
    Node::FromJson(object);

    mAmplitude = object["amplitude"].toDouble();
    mFrequency = object["frequency"].toDouble();
    mOctaves = object["octaves"].toInt();
    mPower = object["power"].toDouble();
    mTessellationMultiplier = object["tessellation_multiplier"].toDouble();
    mGrassCoverage = object["grass_coverage"].toDouble();
    mAmbient = object["ambient"].toDouble();
    mDiffuse = object["diffuse"].toDouble();
    mSpecular = object["specular"].toDouble();
    mShininess = object["shininess"].toDouble();
    mEnabled = object["enabled"].toBool(mEnabled);

    // Seed
    float x = object["seed"]["x"].toDouble();
    float y = object["seed"]["y"].toDouble();
    float z = object["seed"]["z"].toDouble();

    mSeed = QVector3D(x, y, z);
}

Canavar::Engine::Terrain* Canavar::Engine::Terrain::Instance()
{
    static Terrain instance;
    return &instance;
}