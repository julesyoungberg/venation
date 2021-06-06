#include <iostream>
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#include "./app.hpp"
#include "./shader.hpp"

App::~App() {
    glDeleteShader(attractors_vert_shader_);
    glDeleteShader(attractors_frag_shader_);
}

void App::setup() {
    // generate attractors
    attractors_.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

    // setup gpu attractors buffer
    glGenBuffers(1, &vbo_);
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, attractors_.size() * 3 * sizeof(float), 
            attractors_.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // compile attractors shaders
    const char *vert_source = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main(){\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    attractors_vert_shader_ = shader::compile_string(&vert_source, GL_VERTEX_SHADER);
    attractors_frag_shader_ = shader::compile_file("basic.frag", GL_FRAGMENT_SHADER);

    // create attractors program
    attractors_program_ = shader::create_program(
        attractors_vert_shader_, 
        attractors_frag_shader_
    );
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(10.0f);
    glUseProgram(attractors_program_);
    glBindVertexArray(vao_);
    glDrawArrays(GL_POINTS, 0, 3);

    glFlush();
}

