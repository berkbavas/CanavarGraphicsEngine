#include "Material.h"

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
