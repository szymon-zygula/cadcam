#include "viewer.hpp"
#include "ccwindow.hpp"

int main() {

    CCWindow window;
    Viewer viewer(window.x11());

    window.poll(viewer);
}
