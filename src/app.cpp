#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/random.hpp>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#include "./app.hpp"

float rnd() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void App::setup() {
    // generate attractors
    constexpr int num_attractors = 1000;
    for (int i = 0; i < num_attractors; ++i) {
        attractors_.push_back(
            glm::vec2(
                rnd() * 2.0f - 1.0f,
                rnd() * 2.0f - 1.0f
            )
        );
    }
}

void App::draw_attractors() {
    glBegin(GL_POINTS); 
        glPointSize(10.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        for (glm::vec2 v : attractors_) {
            glVertex2f(v.x, v.y);
        }
    glEnd();
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_attractors();

    glFlush();
}

