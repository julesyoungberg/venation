#pragma once

#include <string>
#include <vector>

#include <boost/gil/image.hpp>

#include "venation.hpp"

class App {
    public:
        
        App() = default;
        ~App() = default;

        void setup();
        void update();        
        void draw();

        void toggle_attractors() { show_attractors_ = !show_attractors_; }
        void play_pause() { running_ = !running_; }

        App& configure(unsigned int width, unsigned int height);
        App& seeds(const std::vector<venation::point2>& seeds);
        App& timeout(unsigned int t) { timeout_ = t; return *this; }
        App& num_attractors(unsigned int n) { venation_.num_attractors(n); return *this; }
        App& mode(const std::string& m) { venation_.mode(m); return *this; }
        App& growth_radius(long double r) { venation_.growth_radius(r); return *this; }
        App& growth_rate(long double r) { venation_.growth_rate(r); return *this; }
        App& consume_radius(long double r) { venation_.consume_radius(r); return *this; }
        App& mask_shades(unsigned int s) { mask_shades_ = s; return *this; }
        
        App& set_mask(const boost::gil::rgb8_image_t img);

        unsigned int width() { return width_; }
        unsigned int height() { return height_; }

    private:
        venation venation_;
        unsigned int width_ = 512;
        unsigned int height_ = 512;
        unsigned int timeout_ = 500;
        bool show_attractors_ = true;
        bool running_ = true;
        unsigned int mask_shades_ = 2;

};

