#include <iostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <CGAL/squared_distance_2.h>
#include <GLFW/glfw3.h>

#include "app.hpp"

void App::configure(
    unsigned int width, unsigned int height,
    unsigned int num_attractors, std::string mode, 
    unsigned int timeout, long double growth_radius,
    long double growth_rate, long double consume_radius
) {
    width_ = width;
    height_ = height;
    timeout_ = timeout;
    venation_.configure(width, height, num_attractors, mode, growth_radius, 
        growth_rate, consume_radius);
}

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

