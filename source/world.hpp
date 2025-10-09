#pragma once

#include "base.hpp"
#include "camera.hpp"
#include <vector>

#include <glm/glm.hpp>

struct Sphere {
    glm::vec3 center;
    float     radius;
    glm::vec3 color;
    float     _padding;
};

// TODO(Tejas): Camera Object will be created in this class
class World {
    
public:

    World();
    ~World();

    World(const World&)            = delete;
    World(World&&)                 = delete;
    World& operator=(const World&) = delete;
    World& operator=(World&&)      = delete;

    void updateLightDirection(glm::vec3 dir);
    void addSphere(glm::vec3 center, float radius, glm::vec3 color);
    void addSphere(Sphere sp);

    std::vector<Sphere> getSpheres() const;
    glm::vec3 getLightDirection() const;

public:
    
    Camera camera;

private:

    std::vector<Sphere> m_spheres;
    glm::vec3 m_lightDirection;

};
