
#include "window.hpp"

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

void Window::getSize(int &width, int &height) const {

    if (m_window) {
        glfwGetWindowSize(m_window, &width, &height);
    }
}

void Window::setSize(const int width, const int height) {

    if (m_window) {
        glfwSetWindowSize(m_window, width, height);
    }
}

void Window::getCursorPosition(double& x, double& y) const {

    if (m_window) {
        glfwGetCursorPos(m_window, &x, &y);
    } else {
        x = y = 0.0;
    }
}
