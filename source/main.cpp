
#include "window.hpp"
#include "base.hpp"

#include "shader.hpp"
#include "texture.hpp"

int main() {

    Window *wnd = new Window(1000, 800, "test");

    Texture outputTex(wnd->getWidth(), wnd->getHeight());

    Shader computeShader("../assets/shaders/raytracer_compute.glsl");
    Shader quadShader("../assets/shaders/basic_vert.glsl", "../assets/shaders/basic_frag.glsl");

    float quadVertices[] = {
        // pos           // tex
        -1.0f, -1.0f,    0.0f, 0.0f,
         1.0f, -1.0f,    1.0f, 0.0f,
         1.0f,  1.0f,    1.0f, 1.0f,
        -1.0f,  1.0f,    0.0f, 1.0f
    };
    
    unsigned int quadIndices[] = { 0,1,2, 2,3,0 };
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

    while (!wnd->shouldClose()) {

        int w = wnd->getWidth();
        int h = wnd->getHeight();
        // glViewport(0, 0, w, h);

        double mouseX, mouseY;
        wnd->getCursorPosition(mouseX, mouseY);

        computeShader.bind();
        computeShader.setUniform("resolution", glm::ivec2(w, h));

        glBindImageTexture(0, outputTex.getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        // Work Groups
        glDispatchCompute((GLuint)((w + 15) / 16), (GLuint)((h + 15) / 16), 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        
        quadShader.bind();
        outputTex.bind();

        quadShader.setUniform("screenTex", 0);

        { // Render

            wnd->clear(0xFF00FFFF);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            wnd->swapBuffers();
        }

        wnd->pollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    delete wnd;
    return 0;
}
