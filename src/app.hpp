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
        void draw_attractors();

        std::vector<glm::vec2> attractors_;

};

