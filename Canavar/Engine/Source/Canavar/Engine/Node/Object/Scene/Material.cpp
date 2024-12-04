#include "Material.h"

Canavar::Engine::Material::~Material()
{
    auto textures = mTextures.values();

    for (auto& pTexture : textures)
    {
        delete pTexture;
        pTexture = nullptr;
    }
}

void Canavar::Engine::Material::SetTexture(TextureType type, QOpenGLTexture* texture)
{
    mTextures.insert(type, texture);
}

QOpenGLTexture* Canavar::Engine::Material::GetTexture(TextureType type)
{
    return mTextures.value(type, nullptr);
}

int Canavar::Engine::Material::GetNumberOfTextures()
{
    return mTextures.size();
}
