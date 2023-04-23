#pragma once

#include <iostream>
#include <unordered_map>

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "viewer.hpp"

struct CCWindow {
    GLFWwindow* window;
    std::unordered_map<int, bool> down;
    double myWidth = 100.;
    double myThickness = 100.;

    bool is_key_pressed(int key);

    CCWindow();

    void poll(Viewer& viewer);
    Window x11();
    ~CCWindow();
};
