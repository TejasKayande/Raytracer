
#pragma once

#include "base.hpp"

#include <glm/glm.hpp>
#include <glm/geometric.hpp>


class Camera {

public:

    Camera();
    ~Camera();

    Camera(const Camera&)            = delete;
    Camera(Camera&&)                 = delete;
    Camera& operator=(const Camera&) = delete;
    Camera& operator=(Camera&&)      = delete;

    void moveForward(float delta);
    void moveBackward(float delta);
    void moveLeft(float delta);
    void moveRight(float delta);
    void moveUp(float delta);
    void moveDown(float delta);

    void updateMouse(double ms_x, double ms_y, bool move_cam);

    glm::vec3 getCameraPos() const;
    glm::vec3 getCameraFront() const;
    glm::vec3 getCameraUp() const;

    void setSprinting(bool val);
    bool isSprinting() const;

private:
    
    float m_yaw;
    float m_pitch;
    float m_speed;
    float m_sensitivity;

    glm::vec3 m_camPos;
    glm::vec3 m_camFront;
    glm::vec3 m_camUp;

    bool m_sprinting;

    double m_lastMouseX;
    double m_lastMouseY;
};
