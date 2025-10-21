
#pragma once

#include "base.hpp"

#include "window.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "world.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

class Renderer {

 public:

    Renderer(int wnd_w, int wnd_h);
    ~Renderer();

    Renderer() = delete;

    void updateSSBO(std::vector<Sphere> spheres);

    // TODO(Tejas): create a camera abstraction here
    void draw(int wnd_w, int wnd_h,
              glm::vec3 cam_pos,
              glm::vec3 cam_front,
              glm::vec3 cam_up,
              float     cam_fov,
              glm::vec3 light_dir);

 private:

    GLuint m_VAO, m_VBO, m_EBO, m_SSBO;

    Shader *m_quadShader, *m_computeShader;
    Texture *m_outTexture, *m_skyBoxTexture;

 private:

    void m_initQuad();
};
