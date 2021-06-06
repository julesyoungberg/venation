#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/random.hpp>

#include "./app.hpp"

float rnd() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void App::setup() {
    // generate attractors
    constexpr int num_attractors = 1000;
    std::vector<SDG2::Site_2> sites;
    for (int i = 0; i < num_attractors; ++i) {
        float x = rnd() * 2.0f - 1.0f;
        float y = rnd() * 2.0f - 1.0f;
        attractors_.push_back(glm::vec2(x, y));
        sites.push_back(SDG2::Site_2::construct_site_2(Gt::Point_2(x, y)));
    }

    sdg_.insert(sites.begin(), sites.end(), CGAL::Tag_true());
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

