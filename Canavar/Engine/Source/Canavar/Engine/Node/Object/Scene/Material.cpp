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

bool Canavar::Engine::Material::LoadTextureFromPath(TextureType Type, const QString &Path, int Components)
{
    LOG_DEBUG("Material::LoadTextureFromPath: Loading texture from path: {}", Path.toStdString());
    return LoadTextureFromImage(Type, QImage(Path), Components);
}

bool Canavar::Engine::Material::LoadTextureFromImage(TextureType Type, const QImage &Image, int Components)
{
    if (Image.isNull())
    {
        LOG_WARN("Material::LoadTextureFromImage: Image is null. Returning false.");
        return false;
    }

    QImage Converted;
    GLint InternalFormat;

    if (Components == 1)
    {
        Converted = Image.convertToFormat(QImage::Format_Grayscale8);
        InternalFormat = GL_RED;
    }
    else if (Components == 4)
    {
        Converted = Image.convertToFormat(QImage::Format_RGBA8888);
        InternalFormat = GL_RGBA;
    }
    else
    {
        CGE_EXIT_FAILURE("Material::LoadTexture: Unsupported component size: {}", Components);
    }

    GLuint Id;
    glGenTextures(1, &Id);

    glBindTexture(GL_TEXTURE_2D, Id);
    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Converted.width(), Converted.height(), 0, InternalFormat, GL_UNSIGNED_BYTE, Converted.constBits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    mTextures.insert(Type, Id);

    return true;
}

GLuint Canavar::Engine::Material::GetTexture(TextureType Type) const
{
    return mTextures.value(Type, 0);
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
