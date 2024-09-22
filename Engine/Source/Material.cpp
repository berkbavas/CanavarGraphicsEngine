#include "Material.h"

Canavar::Engine::Material::Material()
    : QObject()
{}

Canavar::Engine::Material::~Material()
{
    // TODO
}

void Canavar::Engine::Material::Insert(TextureType type, QOpenGLTexture* texture)
{
    mTextures.insert(type, texture);
}

QOpenGLTexture* Canavar::Engine::Material::Get(TextureType type)
{
    return mTextures.value(type, nullptr);
}

int Canavar::Engine::Material::GetNumberOfTextures()
{
    return mTextures.size();
}