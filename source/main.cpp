
#include "window.hpp"
#include "base.hpp"

#include "shader.hpp"
#include "texture.hpp"

#include "renderer.hpp"

#include <cstdlib>
#include <ctime>

// TODO(Tejas): Do I want to store them in std::vector?? I dont know...
//              Maybe create a world or scene abstraction!
global std::vector<Sphere> G_spheres;

internal float randRange(float min, float max) {

    return min + static_cast<float>(std::rand()) / RAND_MAX * (max - min);
}

internal bool tooClose(const Sphere& a, const Sphere& b, float minDistance) {

    float distSq = glm::dot(a.center - b.center, a.center - b.center);
    return distSq < (minDistance * minDistance);
}

internal void createSpheres() {
    
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    G_spheres.reserve(20);

    const float worldMin = -100.0f;
    const float worldMax =  100.0f;
    const float minSpacing = 8.0f;  // keep spheres at least 8 units apart

    for (int i = 0; i < 500; ++i) {

        Sphere s;
        bool placed = false;

        s.radius = randRange(1.0f, 7.0f);
        s.color  = glm::vec3(randRange(0.0f, 1.0f),
                             randRange(0.0f, 1.0f),
                             randRange(0.0f, 1.0f));
        s._padding = 0.0f;

        while (!placed) {

            s.center = glm::vec3(randRange(worldMin, worldMax),
                                 randRange(worldMin, worldMax),
                                 randRange(worldMin, worldMax));
            placed = true;
            for (const auto& other : G_spheres) {
                if (tooClose(s, other, minSpacing)) {
                    placed = false;
                    break;
                }
            }
        }

        G_spheres.push_back(s);
    }
}

int main(void) {

    Window   *wnd = new Window(1600, 1600, "test");
    Renderer *rnd = new Renderer(wnd->getWidth(), wnd->getHeight());

    createSpheres();

    // TODO(Tejas): Abstract this away...
    glm::vec3 cam_pos   = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cam_up    = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw         = -90.0f;
    float pitch       = 0.0f;
    float speed       = 8.0f;
    float sensitivity = 0.1f;

    double last_x = wnd->getWidth()  * 0.5;
    double last_y = wnd->getHeight() * 0.5;

    float delta = 0.0f;
    float last  = 0.0f;

    while (!wnd->shouldClose()) {

        int w = wnd->getWidth();
        int h = wnd->getHeight();
        // glViewport(0, 0, w, h);

        float current = (float)wnd->getTime();
        delta = current - last;
        last  = current;

        { // Counting FPS
            persist float accum       = 0.0f;
            persist int   frame_count = 0;
            persist float FPS         = 0.0f;
        
            accum += delta;
            frame_count++;
        
            if (accum >= 1.0f) {
                FPS = frame_count / accum;
                wnd->setTitle("FPS: " + std::to_string(FPS));
                accum = 0.0f;
                frame_count = 0;
            }
        }

        double mouse_x, mouse_y;
        wnd->getCursorPosition(mouse_x, mouse_y);

        { // NOTE(Tejas): Movement Stuff needs to be checked somewhere else.
            
            float xoffset = (float)(mouse_x - last_x);
            float yoffset = (float)(last_y - mouse_y);
            last_x = mouse_x;
            last_y = mouse_y;

            xoffset *= sensitivity;
            yoffset *= sensitivity;

            if (wnd->isMouseButtonPressed(0)) {

                wnd->toggleCursorVisibility(false);
                
                yaw   += xoffset;
                pitch += yoffset;
                if(pitch > 80.0f) pitch = 80.0f;
                if(pitch < -80.0f) pitch = -80.0f;

                cam_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                cam_front.y = sin(glm::radians(pitch));
                cam_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cam_front = glm::normalize(cam_front);

            } else {

                wnd->toggleCursorVisibility(true);
                
            }

            if (wnd->isKeyPressed(GLFW_KEY_LEFT_CONTROL)) speed = 15.0f;
            else speed = 8.0f;

            if (wnd->isKeyPressed(GLFW_KEY_W))          cam_pos += cam_front * speed * delta;   
            if (wnd->isKeyPressed(GLFW_KEY_S))          cam_pos -= cam_front * speed * delta;
            if (wnd->isKeyPressed(GLFW_KEY_A))          cam_pos -= glm::normalize(glm::cross(cam_front, cam_up)) * speed * delta;
            if (wnd->isKeyPressed(GLFW_KEY_D))          cam_pos += glm::normalize(glm::cross(cam_front, cam_up)) * speed * delta;
            if (wnd->isKeyPressed(GLFW_KEY_SPACE))      cam_pos += cam_up * speed * delta;
            if (wnd->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) cam_pos -= cam_up * speed * delta;
        }

        wnd->clear(0x000000FF);
        rnd->updateSSBO(G_spheres);
        rnd->draw(w, h, cam_pos, cam_front, cam_up);
        wnd->swapBuffers();

        wnd->pollEvents();
    }

    delete rnd;
    delete wnd;

    return 0;
}
