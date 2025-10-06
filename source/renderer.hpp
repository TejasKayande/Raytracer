
#pragma once

#include "base.hpp"

#include "window.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Renderer {

 public:

    Renderer();
    ~Renderer();

    void draw(Shader& shader, Texture& texture);

 private:

    GLuint m_VAO, m_VBO, m_EBO;

 private:

    void m_initQuad();
};
