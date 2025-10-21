
#include "texture.hpp"
#include "base.hpp"

#include "stb_image.h"

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

// TODO(Tejas): This is HDR specific. Maybe pass a bool from the parameter, or a enum.
Texture::Texture(const char* file_path) {
    
    // stbi_set_flip_vertically_on_load(false);

    int chnls;

    float* bytes = stbi_loadf(file_path, &m_width, &m_height, &chnls, 0);

    if (!bytes) {
        std::cerr << "coeldnt load " << "\'" << file_path<< "\'\n";
        std::cerr << stbi_failure_reason() << "\n";
        return;
    }

    m_internalFormat = GL_RGB16F;
    m_format = GL_RGB;
    m_type = GL_FLOAT;

    
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat,
                 m_width, m_height, 0,
                 m_format, m_type, bytes);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(bytes);
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

void Texture::bindImage(int unit) const {

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glBindImageTexture(unit, m_textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_internalFormat);
}

void Texture::bindSampler(int unit) const {
    
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::unbind(int unit) const {

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
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
