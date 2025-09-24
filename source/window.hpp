
#pragma once
#include "base.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

class Window {
 public:

    Window(const int width, const int height, const std::string& title);
    ~Window();

    Window(const Window&)            = delete;
    Window(Window&&)                 = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&)      = delete;

    bool shouldClose() const;
    void setShouldClose() const;

    void swapBuffers();
    void pollEvents();
    void clear(Base::Color c);

    void setTitle(const std::string& title);

    int getWidth() const;
    int getHeight() const;
    void setSize(const int width, const int height);

    void getCursorPosition(double& x, double& y) const;

 private:

    GLFWwindow* m_window = nullptr;
};
