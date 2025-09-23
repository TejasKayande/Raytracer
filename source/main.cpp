
#include <iostream>

#include "window.hpp"
#include "base.hpp"

int main() {

    Window *wnd = new Window(1000, 800, "test");

    while (!wnd->shouldClose()) {

        wnd->clear(0xFF00FFFF);

        double x, y;
        wnd->getCursorPosition(x, y);

        std::string s = "(" + std::to_string(int(x)) + ", " + std::to_string(int(y)) + ")";
        wnd->setTitle(s);

        wnd->swapBuffers();
        wnd->pollEvents();
    }

    delete wnd;
    return 0;
}

