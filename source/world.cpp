
#include "world.hpp"

#include "base.hpp"
#include <vector>

World::World() {

    m_spheres.reserve(20);
}

World::~World() {}

void World::updateLightDirection(glm::vec3 dir) {

    m_lightDirection = dir;
}

void World::addSphere(glm::vec3 center, float radius, glm::vec3 color) {

    Sphere s;
    s.center = center;
    s.radius = radius;
    s.color  = color;

    m_spheres.push_back(s);
}

void World::addSphere(Sphere sp) {

    m_spheres.push_back(sp);
}

std::vector<Sphere> World::getSpheres() const {

    return m_spheres;
}

glm::vec3 World::getLightDirection() const {

    return m_lightDirection;
}

std::vector<Sphere>& World::getSpheresRef() {

    return m_spheres;
}
