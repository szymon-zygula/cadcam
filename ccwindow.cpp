#include "ccwindow.hpp"

void error_callback(int, const char* msg) {
    std::cerr << "GLFW Error from CC: " << msg << std::endl;
}

bool CCWindow::is_key_pressed(int key) {
    if (glfwGetKey(window, key) == GLFW_PRESS) {
        if (!down[key]) {
            down[key] = true;
            return true;
        }
    }

    if (glfwGetKey(window, key) == GLFW_RELEASE) {
        down[key] = false;
    }

    return false;
}

CCWindow::CCWindow() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "Could not open GLFW window" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window = glfwCreateWindow(1280, 720, "CAD/CAM", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window!!!" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
}

bool CCWindow::is_running() { return !glfwWindowShouldClose(window); }

void CCWindow::poll(Viewer& viewer) { glfwPollEvents(); }

Window CCWindow::x11() { return glfwGetX11Window(window); }

CCWindow::~CCWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
