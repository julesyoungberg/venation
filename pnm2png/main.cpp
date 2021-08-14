#include <cstdlib>
#include <iostream>

#include <png.h>

#include <boost/algorithm/string.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/pnm.hpp>

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Missing required pnm filename.\n";
        return EXIT_FAILURE;
    }

    std::string filename(argv[1]);
    std::vector<std::string> parts;
    boost::split(parts, filename, boost::is_any_of("."));

    if (parts.size() < 2) {
        std::cerr << "Error: invalid mask file '" << filename
            << "', expected a path to a pnm image file.\n";
        return EXIT_FAILURE;
    }

    std::string extension = parts[parts.size() - 1];
    std::for_each(extension.begin(), extension.end(), [](char & c){
        c = ::tolower(c);
    });
    const char* ext = extension.c_str();

    if (strcmp(ext, "pnm") != 0) {
        std::cerr << "Error: invalid mask file '" << filename
            << "', expected a path to a pnm image file.\n";
        return EXIT_FAILURE;
    }

    boost::gil::rgb8_image_t img;
    boost::gil::read_and_convert_image(filename, img, boost::gil::pnm_tag());
    
    std::string outpath = parts[parts.size() - 2] + ".png";
    boost::gil::write_view(outpath, boost::gil::view(img), 
        boost::gil::png_tag());
    
    return EXIT_SUCCESS;
}
