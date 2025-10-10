
#include "window.hpp"
#include "base.hpp"

#include "shader.hpp"
#include "texture.hpp"

#include "renderer.hpp"

#include <cstdlib>
#include <ctime>

World G_world;

internal float randRange(float min, float max) {

    return min + static_cast<float>(std::rand()) / RAND_MAX * (max - min);
}

internal bool tooClose(const Sphere& a, const Sphere& b, float min_distance) {

    float distSq = glm::dot(a.center - b.center, a.center - b.center);
    return distSq < (min_distance * min_distance);
}

internal void createSpheres() {
    
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const float world_min = -30.0f;
    const float world_max =  30.0f;
    const float min_spacing = 8.0f;
    
#if 0
    for (int i = 0; i < 20; ++i) {

        Sphere s;
        bool placed = false;

        s.radius = randRange(3.0f, 5.0f);
        s.color  = glm::vec3(randRange(0.0f, 1.0f),
                             randRange(0.0f, 1.0f),
                             randRange(0.0f, 1.0f));
        s._padding = 0.0f;

        while (!placed) {

            s.center = glm::vec3(randRange(world_min, world_max),
                                 randRange(world_min, world_max),
                                 randRange(world_min, world_max));
            placed = true;
            for (const auto& other : G_world.getSpheres()) {
                if (tooClose(s, other, min_spacing)) {
                    placed = false;
                    break;
                }
            }

            if (placed) {
                G_world.addSphere(s.center, s.radius, s.color);
            }
        }
    }
#else
    for (int i = 0; i < 20; i++) {
        
        Sphere s;
        s.radius = 1.3f;
        s.color  = glm::vec3(randRange(0.0f, 1.0f),
                             randRange(0.0f, 1.0f),
                             randRange(0.0f, 1.0f));
        s.center = glm::vec3((float)i * 3.0f, 0.0f, 0.0f);
        s._padding = 0.0f;
        G_world.addSphere(s);
    }
#endif

}

int main(void) {

    Window   *wnd = new Window(1600, 1600, "test");
    Renderer *rnd = new Renderer(wnd->getWidth(), wnd->getHeight());

    createSpheres();
    G_world.updateLightDirection(glm::vec3(0.0f, 1.0f, 1.0f));

    float delta = 0.0f;
    float last  = 0.0f;

    while (!wnd->shouldClose()) {

        int w = wnd->getWidth();
        int h = wnd->getHeight();
        // glViewport(0, 0, w, h);

        float current = (float)wnd->getTime();
        delta = current - last;
        last  = current;

        {
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
        if (wnd->isMouseButtonPressed(0)) wnd->toggleCursorVisibility(false);
        else                              wnd->toggleCursorVisibility(true);

        G_world.camera.updateMouse(mouse_x, mouse_y, !wnd->isCursorVisible());

        if (wnd->isKeyPressed(GLFW_KEY_W))          G_world.camera.moveForward(delta);
        if (wnd->isKeyPressed(GLFW_KEY_S))          G_world.camera.moveBackward(delta);
        if (wnd->isKeyPressed(GLFW_KEY_A))          G_world.camera.moveLeft(delta);
        if (wnd->isKeyPressed(GLFW_KEY_D))          G_world.camera.moveRight(delta);
        if (wnd->isKeyPressed(GLFW_KEY_SPACE))      G_world.camera.moveUp(delta);
        if (wnd->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) G_world.camera.moveDown(delta);

        if (wnd->isKeyPressed(GLFW_KEY_UP)) {
            glm::vec3 light_dir = G_world.getLightDirection();
            light_dir.y++;
            G_world.updateLightDirection(light_dir);
        }
        if (wnd->isKeyPressed(GLFW_KEY_DOWN)) {
            glm::vec3 light_dir = G_world.getLightDirection();
            light_dir.y--;
            G_world.updateLightDirection(light_dir);
        }
        if (wnd->isKeyPressed(GLFW_KEY_LEFT)) {
            glm::vec3 light_dir = G_world.getLightDirection();
            light_dir.x--;
            G_world.updateLightDirection(light_dir);
        }
        if (wnd->isKeyPressed(GLFW_KEY_RIGHT)) {
            glm::vec3 light_dir = G_world.getLightDirection();
            light_dir.x++;
            G_world.updateLightDirection(light_dir);
        }

        wnd->clear(0x000000FF);


        persist float accum = 0.0f;
        accum += delta * 2.0f;

        int index = 0;
        for (Sphere& sp : G_world.getSpheresRef()) {

            float phase = index * 0.3f;
            float ampli = 1.0f;
            float base  = 0.5f;

            sp.color.r = base + ampli * sinf(accum + phase);
            sp.color.g = base + ampli * sinf(accum + phase + 2.0f);
            sp.color.b = base + ampli * sinf(accum + phase + 4.0f);

            sp.center.y = base + ampli * sinf(accum + phase + 4.0f);

            index++;
        }

        rnd->updateSSBO(G_world.getSpheres());
        rnd->draw(w, h,
                  G_world.camera.getCameraPos(),
                  G_world.camera.getCameraFront(),
                  G_world.camera.getCameraUp(),
                  G_world.getLightDirection()
             );
        wnd->swapBuffers();

        wnd->pollEvents();
    }

    delete rnd;
    delete wnd;

    return 0;
}
