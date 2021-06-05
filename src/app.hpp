#pragma once

#include <vector>

#include "glm/glm.hpp"

class App {
    public:
        App() = default;
        ~App() = default;

        void setup();
        void draw();

    private:
        std::vector<glm::vec2> attractors_;
        unsigned int vbo_;

};

