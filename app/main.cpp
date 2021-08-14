#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "app.hpp"
#include "growth/venation.hpp"

using namespace growth;

App app;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) {
        return;
    }

    if (key == GLFW_KEY_A) {
        app.toggle_attractors();
    }

    if (key == GLFW_KEY_P) {
        app.play_pause();
    }
}

int main(int argc, const char* argv[]) {
    // parse command line options and configure
    int r = app.parse_options(argc, argv);
    if (r != EXIT_SUCCESS) {
        return r;
    }

    // initialize openGL app
    if (!glfwInit()) { return EXIT_FAILURE; }
    glfwSwapInterval(1);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    app.scale_to_fit(mode->width, mode->height);

    // create window
    GLFWwindow* window = glfwCreateWindow(app.width(), app.height(), argv[0], 
        nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // setup callbacks
    glfwSetKeyCallback(window, key_callback);

    // setup app
    app.window(window);
    app.setup();
    
    // animate"
    while (!glfwWindowShouldClose(window)) {
        app.update();
        glfwMakeContextCurrent(window);
        app.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
