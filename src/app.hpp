#pragma once

#include "venation.hpp"

class App {
    public:
        App() = default;
        ~App() = default;

        void setup();
        void update();
        void draw();

    private:
        venation venation_;

};

