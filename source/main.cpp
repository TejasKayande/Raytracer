
#include "window.hpp"
#include "base.hpp"

#include "shader.hpp"
#include "texture.hpp"

#include "renderer.hpp"

int main(void) {

    Window   *wnd = new Window(1600, 1600, "test");
    Renderer *rnd = new Renderer();

    Texture outputTex(wnd->getWidth(), wnd->getHeight());

    // NOTE(Tejas): There are different uniforms for different shaders here...
    // Shader computeShader("../assets/shaders/basic.compute.glsl");
    // Shader computeShader("../assets/shaders/raytracer.compute.glsl");
    // Shader computeShader("../assets/shaders/cam.compute.glsl");
    Shader computeShader("../assets/shaders/raytri.compute.glsl");
    Shader quadShader("../assets/shaders/basic.vert.glsl", "../assets/shaders/basic.frag.glsl");


    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camUp    = glm::vec3(0.0f, 1.0f, 0.0f);

    // TODO(Tejas): Abstract this away...
    float yaw         = -90.0f;
    float pitch       = 0.0f;
    float speed       = 5.0f;
    float sensitivity = 0.1f;

    double lastX = wnd->getWidth()  * 0.5;
    double lastY = wnd->getHeight() * 0.5;

    float delta = 0.0f;
    float last = 0.0f;

    while (!wnd->shouldClose()) {

        int w = wnd->getWidth();
        int h = wnd->getHeight();
        // glViewport(0, 0, w, h);

        float current = (float)glfwGetTime();
        delta = current - last;
        last  = current;

        { // Counting FPS

            persist float accumulator = 0.0f;
            persist int   frameCount  = 0;
            persist float FPS         = 0.0f;
        
            accumulator += delta;
            frameCount++;
        
            if (accumulator >= 1.0f) {
                FPS = frameCount / accumulator;
                wnd->setTitle("FPS: " + std::to_string(FPS));
                accumulator = 0.0f;
                frameCount = 0;
            }
        }

        double mouseX, mouseY;
        wnd->getCursorPosition(mouseX, mouseY);

        { // Camera and Movement Stuff
            
            float xoffset = (float)(mouseX - lastX);
            float yoffset = (float)(lastY - mouseY);
            lastX = mouseX;
            lastY = mouseY;

            xoffset *= sensitivity;
            yoffset *= sensitivity;

            if (wnd->isMouseButtonPressed(0)) {

                wnd->toggleCursorVisibility(false);
                
                yaw   += xoffset;
                pitch += yoffset;
                if(pitch > 80.0f) pitch = 80.0f;
                if(pitch < -80.0f) pitch = -80.0f;

                camFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                camFront.y = sin(glm::radians(pitch));
                camFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                camFront = glm::normalize(camFront);

            } else {

                wnd->toggleCursorVisibility(true);
                
            }

            if (wnd->isKeyPressed(GLFW_KEY_W))          camPos += camFront * speed * delta;   
            if (wnd->isKeyPressed(GLFW_KEY_S))          camPos -= camFront * speed * delta;
            if (wnd->isKeyPressed(GLFW_KEY_A))          camPos -= glm::normalize(glm::cross(camFront, camUp)) * speed * delta;
            if (wnd->isKeyPressed(GLFW_KEY_D))          camPos += glm::normalize(glm::cross(camFront, camUp)) * speed * delta;
            if (wnd->isKeyPressed(GLFW_KEY_SPACE))      camPos += camUp * speed * delta;
            if (wnd->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) camPos -= camUp * speed * delta;
        }

        { // compute shader

            computeShader.bind();
            computeShader.setUniform("resolution", glm::ivec2(w, h));
            computeShader.setUniform("camPos", camPos);
            computeShader.setUniform("camFront", camFront);
            computeShader.setUniform("camUp", camUp);
        

            // Work Groups
            glDispatchCompute((GLuint)((w + 15) / 16), (GLuint)((h + 15) / 16), 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        { // Render

            wnd->clear(0xFF00FFFF);
            rnd->draw(quadShader, outputTex);
            wnd->swapBuffers();
        }

        wnd->pollEvents();
    }

    delete rnd;
    delete wnd;
    return 0;
}
