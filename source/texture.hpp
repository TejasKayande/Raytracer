
#pragma once

#include "base.hpp"

#include <GL/glew.h>

// FIXME(Tejas): This is right now written considering I only need one texture
//              for the entire screen. Im not going to create multiple instances
//              of this for now.

class Texture {

public:

    Texture(int width, int height);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other);
    Texture& operator=(Texture&& other);

    void bind() const;
    void unbind() const;

    GLuint getID() const;
    int getWidth() const;
    int getHeight() const;

private:

    GLuint m_textureID;
    int    m_width, m_height;
    GLenum m_internalFormat, m_format, m_type;
};
