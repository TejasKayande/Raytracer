
#include "renderer.hpp"

#include "base.hpp"

#include "window.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// NOTE(Tejas): This is for the SSBO
struct _Sphere {
    glm::vec3 center;
    float     radius;
    glm::vec3 color;
    float     _padding;
};

Renderer::Renderer(int wnd_w, int wnd_h) {

    m_quadShader    = new Shader("../assets/shaders/vert.glsl", "../assets/shaders/frag.glsl");
    m_computeShader = new Shader("../assets/shaders/compute.glsl");

    m_outTexture    = new Texture(wnd_w, wnd_h);
    // m_skyBoxTexture = new Texture("../assets/textures/skybox1.hdr");
    m_skyBoxTexture = new Texture("../assets/textures/skybox2.hdr");
    m_initQuad();

}

Renderer::~Renderer() {

    delete m_skyBoxTexture;
    delete m_outTexture;

    delete m_quadShader;
    delete m_computeShader;

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_SSBO);
}

void Renderer::updateSSBO(std::vector<Sphere> spheres) {

    std::vector<_Sphere> sps;
    for (Sphere sp : spheres) {
        _Sphere s;
        s.center = sp.center;
        s.radius = sp.radius;
        s.color  = sp.color;

        s._padding = 0.0f;

        sps.push_back(s);
    }
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sps.size() * sizeof(_Sphere), sps.data(), GL_DYNAMIC_DRAW);

    // NOTE(Tejas): hard coding value here may be okay as there is only going to
    //              be one object of this class.
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SSBO);
}

void Renderer::draw(int wnd_w, int wnd_h, glm::vec3 cam_pos,
                    glm::vec3 cam_front, glm::vec3 cam_up, float cam_fov,
                    glm::vec3 light_dir) {

    m_computeShader->bind();
    m_computeShader->setUniform("resolution", glm::ivec2(wnd_w, wnd_h));
    m_computeShader->setUniform("cam_pos",   cam_pos);
    m_computeShader->setUniform("cam_front", cam_front);
    m_computeShader->setUniform("cam_up",    cam_up);
    m_computeShader->setUniform("cam_fov",    cam_fov);
    m_computeShader->setUniform("light_dir", light_dir);

    m_outTexture->bindImage(0);

    m_skyBoxTexture->bindSampler(2);
    m_computeShader->setUniform("skybox", 2);

    glDispatchCompute((GLuint)((wnd_w + 15) / 16), (GLuint)((wnd_h + 15) / 16), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_quadShader->bind();
    m_outTexture->bindImage(0);
    m_quadShader->setUniform("screenTex", 0);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::m_initQuad() {
    
    float quad_vertices[] = {
        // pos           // tex
        -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f, -1.0f,    1.0f, 0.0f,
         1.0f,  1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f,    0.0f, 1.0f
    };
    
    unsigned int quad_indices[] = { 0,1,2,  2,3,0 };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
        
    glBindVertexArray(m_VAO);
        
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
        
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
        
    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
        
    glBindVertexArray(0);

    // ssbo
    glGenBuffers(1, &m_SSBO);
}
