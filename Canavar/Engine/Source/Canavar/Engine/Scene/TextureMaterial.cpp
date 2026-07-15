#include "TextureMaterial.h"

#include "Canavar/Engine/Util/Logger.h"

Canavar::Engine::TextureMaterial::TextureMaterial(float Opacity)
    : mOpacity(Opacity)
{
    initializeOpenGLFunctions();
}

Canavar::Engine::TextureMaterial::~TextureMaterial()
{
    for (const auto TextureId : mTextures)
    {
        glDeleteTextures(1, &TextureId);
    }

    mTextures.clear();
}

bool Canavar::Engine::TextureMaterial::LoadTexture(TextureType Type, const QImage& Image, int ComponentCount)
{
    if (Image.isNull())
    {
        LOG_WARN("TextureMaterial::LoadTexture: Image is null. Returning false.");
        return false;
    }

    QImage Converted;
    GLint InternalFormat;

    if (ComponentCount == 1)
    {
        Converted = Image.convertToFormat(QImage::Format_Grayscale8);
        InternalFormat = GL_RED;
    }
    else if (ComponentCount == 4)
    {
        Converted = Image.convertToFormat(QImage::Format_RGBA8888);
        InternalFormat = GL_RGBA;
    }
    else
    {
        CGE_EXIT_FAILURE("TextureMaterial::LoadTexture: Unsupported component size: {}", ComponentCount);
    }

    GLuint TextureId;
    glGenTextures(1, &TextureId);

    glBindTexture(GL_TEXTURE_2D, TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Converted.width(), Converted.height(), 0, InternalFormat, GL_UNSIGNED_BYTE, Converted.constBits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    mTextures.insert(Type, TextureId);

    return true;
}

GLuint Canavar::Engine::TextureMaterial::GetTexture(TextureType Type)
{
    return mTextures.value(Type, 0);
}

bool Canavar::Engine::TextureMaterial::HasTexture(TextureType Type) const
{
    return mTextures.value(Type, 0) != 0;
}

int Canavar::Engine::TextureMaterial::GetNumberOfTextures() const
{
    return mTextures.size();
}

float Canavar::Engine::TextureMaterial::GetOpacity() const
{
    return mOpacity;
}
