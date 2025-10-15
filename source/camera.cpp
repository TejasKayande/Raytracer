
#include "camera.hpp"
#include "base.hpp"

#include <glm/glm.hpp>
#include <glm/geometric.hpp>


Camera::Camera() {
    
    m_yaw         = -90.0f;
    m_pitch       = 0.0f;
    m_speed       = 8.0f;
    m_sensitivity = 0.1f;

    m_camPos   = glm::vec3(0.0f, 0.0f, 5.0f);
    m_camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    m_camUp    = glm::vec3(0.0f, 1.0f, 0.0f);

    m_sprinting = false;
}

Camera::~Camera() { }

void Camera::moveForward(float delta) {

    m_camPos += m_camFront * m_speed * delta;
}

void Camera::moveBackward(float delta) {
    
    m_camPos -= m_camFront * m_speed * delta;
}

void Camera::moveLeft(float delta) {
    
    m_camPos -= glm::normalize(glm::cross(m_camFront, m_camUp)) * m_speed * delta;
}

void Camera::moveRight(float delta) {
    
    m_camPos += glm::normalize(glm::cross(m_camFront, m_camUp)) * m_speed * delta;
}

void Camera::moveUp(float delta) {

    m_camPos += m_camUp * m_speed * delta;
}

void Camera::moveDown(float delta) {
    
    m_camPos -= m_camUp * m_speed * delta;
}

void Camera::updateMouse(double ms_x, double ms_y, bool move_cam) {

    float xoffset = (float)(ms_x - m_lastMouseX);
    float yoffset = (float)(m_lastMouseY - ms_y);

    m_lastMouseX = ms_x;
    m_lastMouseY = ms_y;

    xoffset *= m_sensitivity;
    yoffset *= m_sensitivity;

    // FIXME(Tejas): need to do something more robust than this
    if (move_cam) {
        m_yaw   += xoffset;
        m_pitch += yoffset;

        if (m_pitch > 80.0f)  m_pitch = 80.0f;
        if (m_pitch < -80.0f) m_pitch = -80.0f;

        m_camFront.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_camFront.y = sin(glm::radians(m_pitch));
        m_camFront.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

        m_camFront = glm::normalize(m_camFront);
    }
}

glm::vec3 Camera::getCameraPos() const {

    return m_camPos;
}

glm::vec3 Camera::getCameraFront() const {
    
    return m_camFront;
}

glm::vec3 Camera::getCameraUp() const {
    
    return m_camUp;
}

void Camera::setSprinting(bool val) {

    m_sprinting = val;
    if (m_sprinting) {
        m_speed = 20.0f;
    } else {
        m_speed = 8.0f;
    }
}

bool Camera::isSprinting() const {

    return m_sprinting;
}
