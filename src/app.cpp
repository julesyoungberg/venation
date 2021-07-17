#include <iostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <CGAL/squared_distance_2.h>
#include <GLFW/glfw3.h>

#include "app.hpp"

void App::configure(
    unsigned int width, unsigned int height,
    unsigned int num_attractors, const std::string& mode, 
    unsigned int timeout, long double growth_radius,
    long double growth_rate, long double consume_radius,
    const std::vector<venation::point2>& seeds
) {
    width_ = width;
    height_ = height;
    timeout_ = timeout;
    venation_.configure(width, height, num_attractors, mode, growth_radius, 
        growth_rate, consume_radius, seeds);
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

