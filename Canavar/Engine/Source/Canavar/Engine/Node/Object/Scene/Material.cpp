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

bool Canavar::Engine::Material::LoadTextureFromPath(TextureType type, const QString &path, int components)
{
    LOG_DEBUG("Material::LoadTextureFromPath: Loading texture from path: {}", path.toStdString());
    return LoadTextureFromImage(type, QImage(path), components);
}

bool Canavar::Engine::Material::LoadTextureFromImage(TextureType type, const QImage &image, int components)
{
    if (image.isNull())
    {
        LOG_WARN("Material::LoadTextureFromImage: Image is null. Returning false.");
        return false;
    }

    QImage converted;
    GLint internalFormat;

    if (components == 1)
    {
        converted = image.convertToFormat(QImage::Format_Grayscale8);
        internalFormat = GL_RED;
    }
    else if (components == 4)
    {
        converted = image.convertToFormat(QImage::Format_RGBA8888);
        internalFormat = GL_RGBA;
    }
    else
    {
        CGE_EXIT_FAILURE("Material::LoadTexture: Unsupported component size: {}", components);
    }

    GLuint id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, converted.width(), converted.height(), 0, internalFormat, GL_UNSIGNED_BYTE, converted.constBits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    mTextures.insert(type, id);

    return true;
}

GLuint Canavar::Engine::Material::GetTexture(TextureType type)
{
    return mTextures.value(type, 0);
}

bool Canavar::Engine::Material::HasTextureBaseColor() const
{
    return mTextures.value(TextureType::BaseColor, 0);
}

bool Canavar::Engine::Material::HasTextureMetallic() const
{
    return mTextures.value(TextureType::Metallic, 0);
}

bool Canavar::Engine::Material::HasTextureRoughness() const
{
    return mTextures.value(TextureType::Roughness, 0);
}

bool Canavar::Engine::Material::HasTextureAmbientOcclusion() const
{
    return mTextures.value(TextureType::AmbientOcclusion, 0);
}

bool Canavar::Engine::Material::HasTextureNormal() const
{
    return mTextures.value(TextureType::Normal, 0);
}

int Canavar::Engine::Material::GetNumberOfTextures() const
{
    return mTextures.size();
}
