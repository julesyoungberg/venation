#include <cmath>
#include <iostream>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <CGAL/squared_distance_2.h>
#include <GLFW/glfw3.h>

#include "app.hpp"

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

// https://www.generacodice.com/en/articolo/1138144/using-boost-gil-to-convert-an-image-into-8220-raw-8221-bytes
struct PixelInserter {
    std::vector<float>* storage;
    unsigned int n_shades;

    PixelInserter(std::vector<float>* s, unsigned int shades): storage(s), n_shades(shades) {}

    void operator()(boost::gil::rgb8_pixel_t p) const {
        float r = (float)boost::gil::at_c<0>(p) / 255.0f;
        float g = (float)boost::gil::at_c<1>(p) / 255.0f;
        float b = (float)boost::gil::at_c<2>(p) / 255.0f;

        float a = (r + g + b) / 3.0;
        float quantized = round((a * (float)n_shades));
        float result = quantized / n_shades;

        storage->push_back(result);
    }
};

App& App::set_mask(const boost::gil::rgb8_image_t img) {
    std::vector<float> img_data;
    img_data.reserve(img.width() * img.height());
    boost::gil::for_each_pixel(boost::gil::const_view(img), PixelInserter(&img_data, mask_shades_));
    configure(img.width(), img.height());
    venation_.mask_data(img_data);
    return *this;
}

void App::setup() {
    venation_.generate_attractors();
    venation_.create_seeds();
}

void App::update() {
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

