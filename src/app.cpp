#include <iostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <CGAL/squared_distance_2.h>
#include <GLFW/glfw3.h>

#include "app.hpp"

void App::setup() {
    venation_.generate_attractors();
    venation_.create_seed();
}

void App::update() {
    venation_.update();
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    venation_.draw_attractors();
    venation_.draw_nodes();

    glFlush();
}

