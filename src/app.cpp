#include <algorithm>
#include <iostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>
#include <glm/gtc/random.hpp>

#include "app.hpp"

double rnd() {
    return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

void App::generate_attractors() {
    constexpr int num_attractors = 500;
    double x;
    double y;
    attractors_ = std::vector<point2>(num_attractors);

    for (int i = 0; i < num_attractors; ++i) {
        x = rnd() * 2.0 - 1.0;
        y = rnd() * 2.0 - 1.0;
        attractors_[i] = point2(x, y);
    }

    attractors_graph_.insert(attractors_.begin(), attractors_.end());
}

void App::create_seed() {
    point2 seed_point(0.0, 0.0);
    node_ref seed_node = node::create(seed_point);
    nodes_.push_back(seed_node);
    nodes_graph_.insert(seed_point);
}

void App::setup() {
    generate_attractors();
}

void App::draw_attractors() {
    point2 query(0.0, 0.0);
    auto nearest_v = attractors_graph_.nearest_vertex(query);
    auto nearest_p = nearest_v->point();
    std::cout << "neareset: " << nearest_p << std::endl;
    attractors_.erase(std::remove(attractors_.begin(), attractors_.end(), nearest_p),
            attractors_.end());
    attractors_graph_.remove(nearest_v);

    glPointSize(10.0f);
    glBegin(GL_POINTS); 
        glColor3f(1.0f, 0.0f, 0.0f);
        for (const auto& v : attractors_) {
            glVertex2d(v.x(), v.y());
        }
    glEnd();
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_attractors();
    
    // glFlush();
}

