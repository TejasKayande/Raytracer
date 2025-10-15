
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

    const float world_min = -300.0f;
    const float world_max =  300.0f;
    const float min_spacing = 8.0f;

    const int number_of_spheres = 200;
#if 1
    for (int i = 0; i < number_of_spheres; ++i) {

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
    for (int i = 0; i < number_of_spheres; i++) {
        
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

        if (wnd->isKeyPressed(GLFW_KEY_F)) {
            Sphere s;
            s.radius = randRange(6.0f, 20.0f);
            s.color  = glm::vec3(randRange(0.0f, 1.0f),
                                 randRange(0.0f, 1.0f),
                                 randRange(0.0f, 1.0f));
            s.center = G_world.camera.getCameraPos();
            s._padding = 0.0f;
            G_world.addSphere(s);
        }

        G_world.camera.updateMouse(mouse_x, mouse_y, !wnd->isCursorVisible());

        if (wnd->isKeyPressed(GLFW_KEY_LEFT_CONTROL)) G_world.camera.setSprinting(true);
        else G_world.camera.setSprinting(false);

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

        float gravity  = 100.0f;

        auto& spheres = G_world.getSpheresRef();

        size_t n = spheres.size();
        for (int i = 0; i < n; ++i) {
            glm::vec3 total_force = glm::vec3(0.0f);

            for (int j = 0; j < n; ++j) {
                if (i == j) continue;

                glm::vec3 dir = spheres[j].center - spheres[i].center;
                float dist_sq = glm::dot(dir, dir);
                if (dist_sq < 0.01f) continue;

                float inv_dist = 1.0f / sqrtf(dist_sq);
                dir *= inv_dist;

                float force_mag = gravity / dist_sq;

                total_force += dir * force_mag;
            }

            spheres[i].center += total_force * delta;
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
