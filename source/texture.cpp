
#include "texture.hpp"
#include "base.hpp"

#include <GL/glew.h>

Texture::Texture(int width, int height)
    : m_width(width), m_height(height),
      m_internalFormat(GL_RGBA32F),
      m_format(GL_RGBA), m_type(GL_FLOAT)
{

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, m_type, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);   
}

Texture::~Texture() {

    if (m_textureID) {
        glDeleteTextures(1, &m_textureID);
    }
}

Texture::Texture(Texture&& other)
    : m_textureID(other.m_textureID),
      m_width(other.m_width), m_height(other.m_height),
      m_internalFormat(other.m_internalFormat),
      m_format(other.m_format), m_type(other.m_type)
{
    other.m_textureID = 0;
}

Texture& Texture::operator=(Texture&& other) {

    if (this != &other) {

        if (m_textureID) glDeleteTextures(1, &m_textureID);

        m_textureID      = other.m_textureID;
        m_width          = other.m_width;
        m_height         = other.m_height;
        m_internalFormat = other.m_internalFormat;
        m_format         = other.m_format;
        m_type           = other.m_type;

        other.m_textureID = 0;
    }

    return *this;
}

void Texture::bind(int unit) const {

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glBindImageTexture(0, m_textureID, 0, m_type, 0, GL_WRITE_ONLY, m_internalFormat);
}

void Texture::unbind(int unit) const {

    glBindTexture(GL_TEXTURE_2D, 0 + unit);
}

GLuint Texture::getID() const {

    return m_textureID;
}

int Texture::getWidth() const {

    return m_width;
}

int Texture::getHeight() const {

    return m_height;
}
