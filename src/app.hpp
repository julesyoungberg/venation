#pragma once

#include <string>
#include <vector>

#include "venation.hpp"

class App {
    public:
        App() = default;
        ~App() = default;

        App& configure(unsigned int width, unsigned int height,
            const std::vector<venation::point2>& seeds);
        void setup();
        void update();
        void draw();

        void toggle_attractors() { show_attractors_ = !show_attractors_; }
        void play_pause() { running_ = !running_; }

        unsigned int width() { return width_; }
        unsigned int height() { return height_; }

        App& timeout(unsigned int t) { timeout_ = t; return *this; }
        App& num_attractors(unsigned int n) { venation_.num_attractors(n); return *this; }
        App& mode(const std::string& m) { venation_.mode(m); return *this; }
        App& growth_radius(long double r) { venation_.growth_radius(r); return *this; }
        App& growth_rate(long double r) { venation_.growth_rate(r); return *this; }
        App& consume_radius(long double r) { venation_.consume_radius(r); return *this; }

    private:
        venation venation_;
        unsigned int width_ = 512;
        unsigned int height_ = 512;
        unsigned int timeout_ = 500;
        bool show_attractors_ = true;
        bool running_ = true;

};

