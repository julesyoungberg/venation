#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/random.hpp>

#include "./app.hpp"

double rnd() {
    return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

void App::generate_attractors() {
    constexpr int num_attractors = 1000;
    std::vector<SDG2::Site_2> sites;
    double x;
    double y;
    Gt::Point_2 p;

    for (int i = 0; i < num_attractors; ++i) {
        x = rnd() * 2.0 - 1.0;
        y = rnd() * 2.0 - 1.0;
        p = Gt::Point_2(x, y);
        attractors_.push_back(p);
        sites.push_back(SDG2::Site_2::construct_site_2(p));
    }

    attractors_sdg_.insert(sites.begin(), sites.end(), CGAL::Tag_true());
}

void App::setup() {
    generate_attractors();
}

void App::draw_attractors() {
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

    // draw_attractors();
    Gt::Point_2 p(0.0, 0.0);
    SDG2::Vertex_handle v = attractors_sdg_.nearest_neighbor(p);
    std::cout << "vertex: " << v->site() << std::endl;

    glFlush();
}

