#include <cstdlib>
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
    // glutInit(&argc, argv);
    if (!glfwInit()) { return EXIT_FAILURE; }
    glfwSwapInterval(1);

    // create window
    // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    // glutInitWindowSize(512, 512);
    // glutCreateWindow(argv[0]);
    GLFWwindow* window = glfwCreateWindow(512, 512, argv[0], nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    app.setup();
    
    // animate
    // glutDisplayFunc(display);
    // glutMainLoop();
    glfwSetWindowRefreshCallback(window, display);
    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}

