#pragma once

#include <string>
#include <vector>

#include "venation.hpp"

class App {
    public:
        App() = default;
        ~App() = default;

        void configure(unsigned int width, unsigned int height,
            unsigned int num_attractors, const std::string& mode, 
            unsigned int timeout, long double growth_radius,
            long double growth_rate, long double consume_radius,
            std::vector<venation::point2>& seeds);
        void setup();
        void update();
        void draw();

        unsigned int width() { return width_; }
        unsigned int height() { return height_; }

    private:
        venation venation_;
        unsigned int width_ = 512;
        unsigned int height_ = 512;
        unsigned int timeout_ = 500;

};

