#pragma once

#include <chrono>
#include <string>
#include <vector>

#include <boost/gil/image.hpp>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "growth/venation.hpp"

using namespace growth;

// This class manages the execution of the venation simulation
// and other openGL, input, and output related things.
class App {
    public:
        
        App() = default;
        ~App() = default;

        // lifecycle
        int parse_options(int argc, const char* argv[]);
        void setup();
        void update();        
        void draw();

        // modifiers
        void toggle_attractors() { show_attractors_ = !show_attractors_; }
        void play_pause() { running_ = !running_; }

        // setters
        App& window(GLFWwindow* w) { window_ = w; return *this; }
        App& mask(const boost::gil::rgb8_image_t img);

        // getters
        unsigned int width() { return venation_.width(); }
        unsigned int height() { return venation_.height(); }

    private:
        void check_timeout();

        venation venation_;
        unsigned int timeout_ = 60;
        bool show_attractors_ = false;
        bool running_ = true;
        std::string out_file_;
        std::chrono::time_point<std::chrono::system_clock> start_;
        GLFWwindow* window_;

};

