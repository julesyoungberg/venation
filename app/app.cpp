#include <cmath>
#include <cstdlib>
#include <iostream>

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <CGAL/squared_distance_2.h>

#include "img.hpp"

#include "app.hpp"

using namespace growth;

App& App::configure(unsigned int width, unsigned int height) {
    width_ = width;
    height_ = height;
    venation_.configure(width, height);
    return *this;
}

App& App::seeds(const std::vector<venation::point2>& seeds) {
    venation_.seeds(seeds);
    return *this;
}

App& App::mask(const boost::gil::rgb8_image_t img) {
    // Reconfigure. The input image's dimensions trump any configuration.
    configure(img.width(), img.height());
    venation_.mask(img);
    return *this;
}

void App::setup() {
    venation_.generate_attractors();
    venation_.create_seeds();

    if (timeout_ > 0.0) {
        start_ = std::chrono::system_clock::now();
    }
}

void App::check_timeout() {
    if (!running_ || timeout_ == 0.0) {
        return;
    }

    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = now - start_;
    double running_time = diff.count();
    double timeout = (double)timeout_;

    if (running_time >= (double)timeout_) {
        // if an out file was provided,
        // write to it and exit
        if (!out_file_.empty()) {
            img::save_frame(out_file_, window_);
            exit(EXIT_SUCCESS);
        } else {
            // otherwise stop the simulation
            running_ = false;
        }
    }
}

void App::update() {
    check_timeout();

    if (!running_) {
        return;
    }

    venation_.update();
}

void App::draw() {
    if (!running_) {
        return;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (show_attractors_) {
        venation_.draw_attractors();
    }

    venation_.draw_nodes();

    glFlush();
}

