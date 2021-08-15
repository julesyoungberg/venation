/**
 * Some image-related helpers
 */
#pragma once

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/pnm.hpp>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

namespace img {

    /**
     * A callable struct that facilitates the conversion of a boost GIL image 
     * object into a vector of pixel data. This is based on the below link:
     * https://www.generacodice.com/en/articolo/1138144/using-boost-gil-to-convert-an-image-into-8220-raw-8221-bytes
     * In this case I have it convert the pixel to a single value instead 
     * of storing each channel.
     * Template parameter R must be a real number like float, double, long dobule.
     */
    template <class R>
    struct BlackAndWhitePixelInserter {
        std::vector<R>* storage;
        unsigned int n_shades;

        BlackAndWhitePixelInserter(std::vector<R>* s, unsigned int shades): storage(s), n_shades(shades) {}

        void operator()(boost::gil::rgb8_pixel_t p) const {
            // get pixel values
            R r = (R)boost::gil::at_c<0>(p) / R(255);
            R g = (R)boost::gil::at_c<1>(p) / R(255);
            R b = (R)boost::gil::at_c<2>(p) / R(255);

            // get quantized brightness
            R a = (r + g + b) / R(3);
            R quantized = round((a * (R)n_shades));
            R result = quantized / n_shades;

            // store result
            storage->push_back(result);
        }
    };

    /**
     * Saves the current openGL frame buffer to the file given by filepath.
     */
    inline void save_frame(const std::string& filepath, GLFWwindow* window) {
        std::cout << "Saving frame...\n";

        // get window size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadBuffer(GL_FRONT);
        
        // get pixel data
        GLubyte* data = (GLubyte*)malloc(width * height * 3);
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

        // prepare image object
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

        // save image to disk
        boost::gil::write_view(filepath, boost::gil::view(img), 
            boost::gil::pnm_tag());

        std::cout << "Output written to " << filepath << '\n';
    }

}
