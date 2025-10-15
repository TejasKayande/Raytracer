
#include "window.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

Window::Window(const int width, const int height, const std::string& title) {

    if (!glfwInit()) {
        std::cerr << "Failed to Initialize GLFW\n";
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);

    glfwSwapInterval(0);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: "
                  << glewGetErrorString(err) << "\n";
        glfwDestroyWindow(m_window);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
}

Window::~Window() {

    if (m_window) glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::shouldClose() const {

    return glfwWindowShouldClose(m_window);
}

void Window::setShouldClose() const {

    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void Window::swapBuffers() {

    glfwSwapBuffers(m_window);
}

void Window::pollEvents() {

    glfwPollEvents();
}

void Window::clear(Base::Color c) {

    u8 r = (c >> 24) & 0xFF;
    u8 g = (c >> 16) & 0xFF;
    u8 b = (c >> 8)  & 0xFF;
    u8 a = (c)       & 0xFF;

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::setTitle(const std::string& title) {

    if (m_window) {
        glfwSetWindowTitle(m_window, title.c_str());
    }
}

int Window::getWidth() const {

    if (m_window) {
        int width;
        glfwGetWindowSize(m_window, &width, NULL);
        return width;
    }

    return 0;
}

int Window::getHeight() const {

    if (m_window) {
        int height;
        glfwGetWindowSize(m_window, NULL, &height);
        return height;
    }

    return 0;
}

void Window::setSize(const int width, const int height) {

    if (m_window) {
        glfwSetWindowSize(m_window, width, height);
    }
}

double Window::getTime() const {

    if (m_window) return glfwGetTime();
    return -1.0;
}

void Window::getCursorPosition(double& x, double& y) const {

    if (m_window) glfwGetCursorPos(m_window, &x, &y);
    else x = y = 0.0;
}

bool Window::isKeyPressed(int key) const {
 
    if (!m_window) return false;
    return glfwGetKey(m_window, key) == GLFW_PRESS;   
}

bool Window::isMouseButtonPressed(int button) const {
 
    if (!m_window) return false;
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;   
}

void Window::toggleCursorVisibility(bool val) {

    m_cursorVisibility = val;

    if (m_cursorVisibility)
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

bool Window::isCursorVisible() const {

    return m_cursorVisibility;
}
