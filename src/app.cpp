#include <iostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <CGAL/squared_distance_2.h>
#include <GLFW/glfw3.h>

#include "app.hpp"

App& App::configure(
    unsigned int width, unsigned int height,
    const std::vector<venation::point2>& seeds
) {
    width_ = width;
    height_ = height;
    venation_.configure(width, height, seeds);
    return *this;
}

void App::setup() {
    venation_.generate_attractors();
    venation_.create_seeds();
}

void App::update() {
    if (!running_) {
        return;
    }

    venation_.update();
}

void App::draw() {
    if (!running_) {
        return;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (show_attractors_) {
        venation_.draw_attractors();
    }

    venation_.draw_nodes();

    glFlush();
}

