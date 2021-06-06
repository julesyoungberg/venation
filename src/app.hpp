#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "./shader.hpp"

class App {
    public:
        App() = default;
        ~App();

        void setup();
        void draw();

    private:
        std::vector<glm::vec3> attractors_;
        unsigned int vbo_;
        unsigned int vao_;
        unsigned int attractors_program_;

        unsigned int attractors_vert_shader_;
        unsigned int attractors_frag_shader_;
};

