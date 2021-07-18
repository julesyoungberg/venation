#include <cmath>
#include <cstdlib>
#include <iostream>

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <CGAL/squared_distance_2.h>

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

    if (timeout_ > 0.0) {
        start_ = std::chrono::system_clock::now();
    }
}

// https://lencerf.github.io/post/2019-09-21-save-the-opengl-rendering-to-image-file/
void save_frame(const std::string& filepath, GLFWwindow* window) {
    std::cout << "Saving frame...\n";

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    
    GLubyte* data = (GLubyte*)malloc(width * height * 3);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    boost::gil::rgb8_image_t img(width, height);
    const boost::gil::rgb8_view_t& viewer = boost::gil::view(img);
    
    // copy pixel data into boost image object
    for (int y = 0; y < viewer.height(); ++y) {
        boost::gil::rgb8_view_t::x_iterator row = viewer.row_begin(y);
        
        for (int x = 0; x < viewer.width(); ++x) {
            unsigned int index = ((height - y) * width + x) * 3;
            boost::gil::at_c<0>(row[x]) = data[index];
            boost::gil::at_c<1>(row[x]) = data[index + 1];
            boost::gil::at_c<2>(row[x]) = data[index + 2];
        }
    }

    boost::gil::write_view(filepath, boost::gil::view(img), 
        boost::gil::png_tag());

    std::cout << "Output written to " << filepath << ".\n";
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
            save_frame(out_file_, window_);
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

