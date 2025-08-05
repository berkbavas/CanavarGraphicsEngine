#include "Terrain.h"

#include "Canavar/Engine/Core/Shader.h"
#include "Canavar/Engine/Node/Object/Camera/Camera.h"
#include "Canavar/Engine/Util/ModelImporter.h"

#include <QMatrix4x4>

Canavar::Engine::Terrain::Terrain()
{
    SetNodeName("Terrain");
}

void Canavar::Engine::Terrain::Initialize()
{
    initializeOpenGLFunctions();

    Reset();

    mTileGenerator = new TileGenerator(4, 64, 2048.0f);

    mTextures.insert("Terrain", ModelImporter::CreateTexture(":/Resources/Terrain/grass2.jpg"));
    mTextures.insert("Grass", ModelImporter::CreateTexture(":/Resources/Terrain/grass0.jpg"));
    mTextures.insert("Sand", ModelImporter::CreateTexture(":/Resources/Terrain/sand0.jpg"));
    mTextures.insert("Snow", ModelImporter::CreateTexture(":/Resources/Terrain/grass1.jpg"));
    mTextures.insert("Rock", ModelImporter::CreateTexture(":/Resources/Terrain/rock3.jpg"));
}

void Canavar::Engine::Terrain::Render(Shader* pShader, Camera* pCamera)
{
    if (mEnabled == false)
    {
        return;
    }

    QVector2D currentTilePosition = mTileGenerator->WhichTile(pCamera->GetWorldPosition());

    if (currentTilePosition != mPreviousTilePosition)
    {
        mTileGenerator->TranslateTiles((currentTilePosition - mPreviousTilePosition));
        mPreviousTilePosition = currentTilePosition;

        qDebug() << "Terrain::Render:" << "currentTilePosition:" << currentTilePosition;
    }

    pShader->Bind();
    pShader->SetUniformValue("nodeId", static_cast<float>(GetNodeId()));
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
    pShader->SetUniformValue("waterHeight", mWaterHeight);
    pShader->SetSampler("sand", 1, mTextures.value("Sand")->textureId());
    pShader->SetSampler("grass0", 2, mTextures.value("Grass")->textureId());
    pShader->SetSampler("grass1", 3, mTextures.value("Terrain")->textureId());
    pShader->SetSampler("rock", 5, mTextures.value("Rock")->textureId());
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
    mGrassCoverage = 0.25f;
    mSeed = QVector3D(1, 1, 1);

    mAmbient = 0.2f;
    mDiffuse = 0.85f;
    mShininess = 8.0f;
    mSpecular = 0.0f;
}

void Canavar::Engine::Terrain::ToJson(QJsonObject& object)
{
    Global::ToJson(object);

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

void Canavar::Engine::Terrain::FromJson(const QJsonObject& object, const QSet<NodePtr>& nodes)
{
    Global::FromJson(object, nodes);

    mAmplitude = object["amplitude"].toDouble(20.0f);
    mFrequency = object["frequency"].toDouble(0.01f);
    mOctaves = object["octaves"].toInt(13);
    mPower = object["power"].toDouble(3.0f);
    mTessellationMultiplier = object["tessellation_multiplier"].toDouble(1.0f);
    mGrassCoverage = object["grass_coverage"].toDouble(0.25f);
    mAmbient = object["ambient"].toDouble(0.2f);
    mDiffuse = object["diffuse"].toDouble(0.85f);
    mSpecular = object["specular"].toDouble(0.0f);
    mShininess = object["shininess"].toDouble(8.0f);
    mEnabled = object["enabled"].toBool(true);

    // Seed
    float x = object["seed"]["x"].toDouble();
    float y = object["seed"]["y"].toDouble();
    float z = object["seed"]["z"].toDouble();
    mSeed = QVector3D(x, y, z);
}
