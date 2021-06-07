#include <cstdlib>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "app.hpp"

static App app;

void display(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
    app.draw();
    glfwSwapBuffers(window);
}

int main(int argc, char** argv) {
    // initialize app
    if (!glfwInit()) { return EXIT_FAILURE; }
    glfwSwapInterval(1);

    // create window
    GLFWwindow* window = glfwCreateWindow(512, 512, argv[0], nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    app.setup();
    
    // animate
    while (!glfwWindowShouldClose(window)) {
        app.update();
        display(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}

