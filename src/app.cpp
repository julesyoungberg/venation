#include <iostream>
#include <GLFW/glfw3.h>

#include "./app.hpp"
#include "./shader.hpp"

void App::setup() {
    attractors_.push_back(glm::vec2(0.0f, 0.0f));

    // std::cout << "creating vbo\n";
    glGenBuffers(1, &vbo_);
    // std::cout << "binding vbo\n";
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    // std::cout << "buffering data\n";
    // glBufferData(GL_ARRAY_BUFFER, sizeof(attractors_), attractors_.data(), GL_DYNAMIC_DRAW);

    // std::cout << "creating vertex shader\n";
    // shader vert_shader = shader("basic.vert");
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(0.5f, 0.5f);
    glEnd();

    glFlush();
}

