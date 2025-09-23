
#include "window.hpp"
#include "base.hpp"

#include "shader.hpp"

int main() {

    Window *wnd = new Window(1000, 800, "test");

    Shader shader("../assets/shaders/basic_vert.glsl", "../assets/shaders/basic_frag.glsl");
    shader.bind();

    float quadVertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    while (!wnd->shouldClose()) {

        wnd->clear(0xFF00FFFF);

        double mouseX, mouseY;
        wnd->getCursorPosition(mouseX, mouseY);
        shader.setUniform("uMouse", glm::vec2(float(mouseX), float(mouseY)));

        shader.setUniform("uResolution", glm::vec2(1000.0f, 800.0f));

        shader.bind();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        wnd->swapBuffers();
        wnd->pollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete wnd;
    return 0;
}

