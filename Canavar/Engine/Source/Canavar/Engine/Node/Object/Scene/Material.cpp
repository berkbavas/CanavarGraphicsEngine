#include "Material.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::Material::Material()
{
    initializeOpenGLFunctions();
}

Canavar::Engine::Material::~Material()
{
    CGE_LOG_ENTER_EXIT("Material::~Material");

    const auto textures = mTextures.values();

    for (const auto texture : textures)
    {
        glDeleteTextures(1, &texture);
    }
}

void Canavar::Engine::Material::LoadTexture(TextureType type, const QImage& image)
{
    const auto converted = image.convertToFormat(QImage::Format_RGBA8888);

    GLuint id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, converted.width(), converted.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, converted.constBits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    mTextures.insert(type, id);
}

GLuint Canavar::Engine::Material::GetTexture(TextureType type)
{
    return mTextures.value(type, 0);
}

bool Canavar::Engine::Material::HasNormalTexture() const
{
    return mTextures.value(TextureType::Normal, 0);
}

bool Canavar::Engine::Material::HasAnyColorTexture() const
{
    return mTextures.value(TextureType::Ambient, 0) || mTextures.value(TextureType::Diffuse, 0) || mTextures.value(TextureType::BaseColor, 0);
}

int Canavar::Engine::Material::GetNumberOfTextures() const
{
    return mTextures.size();
}
