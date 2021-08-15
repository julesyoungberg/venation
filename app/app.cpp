#include <cmath>
#include <cstdlib>
#include <iostream>
#include <regex>

#include <boost/algorithm/string.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/pnm.hpp>
#include <boost/program_options.hpp>
#include <CGAL/squared_distance_2.h>

#include "img.hpp"

#include "app.hpp"

namespace po = boost::program_options;

using namespace growth;

/**
 * Erases all instances of a char from a string.
 * Source: https://stackoverflow.com/questions/20326356/how-to-remove-all-the-occurrences-of-a-char-in-c-string
 */
void erase_all(std::string& str, char c) {
    if (str.find(c) != std::string::npos) {
        str.erase(std::remove(str.begin(), str.end(), c), str.end());
    }
}

int App::parse_options(int argc, const char* argv[]) {
    unsigned int width = venation_.width();
    unsigned int height = venation_.height();
    boost::gil::rgb8_image_t mask_img;
    bool valid_mask_provided = false;
    
    // parse command line options
    try {
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "produce help message")
            ("width", po::value<unsigned int>(), 
                "Simulation width in pixels. Defaults to 512, overridden by mask.")
            ("height", po::value<unsigned int>(), 
                "Simulation height in pixels. Defaults to 512, overridden by mask.")
            ("num-attractors", po::value<unsigned int>(),
                "Number of random attractors to generate. Defaults to 5000.")
            ("seeds", po::value<std::string>(),
                "A list of 2D points to start growing from. Input should be of "
                "the form \"(x1,y2),...,(xn,yn)\" where each x and y is in "
                "the interval [-1, 1]. Defaults to (0,0).")
            ("mode", po::value<std::string>(),
                "Growth mode, 'open' or 'closed' venation styles. "
                "Defaults to 'open'.")
            ("timeout", po::value<unsigned int>(),
                "A time limit in seconds after which the simulation result "
                "will be saved to the output file and the program will "
                "terminate. Defaults to 60 seconds.")
            ("growth-radius", po::value<long double>(),
                "The maximum distance an attractor can be from a growth node "
                "and still influence it (relative to normalized points). "
                "Defaults to 0.1.")
            ("growth-rate", po::value<long double>(),
                "The size of the step taken at each growth step (relative to "
                "normalized points). Defaults to 0.002.")
            ("consume-radius", po::value<long double>(),
                "The distance between an attractor and node at which point "
                "the attractor is considered consumed and removed "
                "(relative to normalized points). Defaults to 0.0005.")
            ("mask-shades", po::value<unsigned int>(),
                "The number of grayscale shades to quantize the mask down to. "
                "Defaults to 2.")
            ("mask", po::value<std::string>(),
                "A path to a pnm image file that will be used to mask "
                "the attractors. i.e. generated attractors will only be kept "
                "for the simulation if the corresponding pixel in the image "
                "is bright enough. Simple, black and white images are best."
                "The file is converted to grayscale and quantized into "
                "`mask-shades` shades. The grayscale value is then used as a "
                "probability that an attractor at that position will be kept.")
            ("outfile", po::value<std::string>(), 
                "An image path to store the result at. The path must include "
                "an extension and it must be pnm.");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Venation Pattern Generator\n\n"
                << "This program simulates venation growth using a "
                << "space colonization algorithm.\n\n";
            std::cout << desc << '\n';
            return EXIT_FAILURE;
        }

        if (vm.count("width")) {
            width = vm["width"].as<unsigned int>();
        }

        if (vm.count("height")) {
            height = vm["height"].as<unsigned int>();
        }

        if (vm.count("num-attractors")) {
            venation_.num_attractors(vm["num-attractors"].as<unsigned int>());
        }

        if (vm.count("seeds")) {
            std::vector<venation::point2> seeds;

            // validate the input with a regex
            std::string floating_point_regex = "[+-]?([0-9]*[.])?[0-9]+";
            std::string point_regex = "\\(" + floating_point_regex + "," + floating_point_regex + "\\)";
            std::string seeds_regex = "^" + point_regex + "(," + point_regex + ")*$";
            std::regex regex(seeds_regex);
            
            std::string input = vm["seeds"].as<std::string>();
            if (std::regex_match(input, regex) == 0) {
                std::cout << "Invalid seeds input, expected string of the form "
                    << "\"(x1,y1),...,(xn,yn)\", example: \"(-0.5,0.9),(0.1,0.25)\"\n";
                return EXIT_FAILURE;
            }

            // parse the input
            std::vector<std::string> point_strings;
            std::size_t pos = 0;
            std::string delimiter = "),(";

            // find each occurence of the delimiter
            while ((pos = input.find(delimiter)) != std::string::npos) {
                point_strings.push_back(input.substr(0, pos));
                input.erase(0, pos + delimiter.length());
            }

            // push the last part of the string
            point_strings.push_back(input);

            // parse each point string
            for (auto& point_str : point_strings) {
                // remove leftover parenthesis
                erase_all(point_str, '(');
                erase_all(point_str, ')');

                // find the , that splits the x and y coord
                std::size_t pos = point_str.find(",");
                if (pos == std::string::npos) {
                    continue;
                }

                // read x and y coords from string
                std::string x_token = point_str.substr(0, pos);
                std::string y_token = point_str.substr(pos + 1, point_str.length());
                double x = std::stod(x_token);
                double y = std::stod(y_token);

                // save point in seeds vector
                seeds.push_back(venation::point2(x, y));
            }

            venation_.seeds(seeds);
        }

        if (vm.count("mode")) {
            venation_.mode(vm["mode"].as<std::string>());
        }

        if (vm.count("timeout")) {
            timeout_ = vm["timeout"].as<unsigned int>();
        }

        if (vm.count("growth-radius")) {
            venation_.growth_radius(vm["growth-radius"].as<long double>());
        }

        if (vm.count("growth-rate")) {
            venation_.growth_rate(vm["growth-rate"].as<long double>());
        }

        if (vm.count("consume-radius")) {
            venation_.consume_radius(vm["consume-radius"].as<long double>());
        }

        if (vm.count("mask-shades")) {
            venation_.mask_shades(vm["mask-shades"].as<unsigned int>());
        }

        if (vm.count("mask")) {
            // split the filename to check the extension
            std::string mask_file = vm["mask"].as<std::string>();
            std::vector<std::string> parts;
            boost::split(parts, mask_file, boost::is_any_of("."));

            if (parts.size() < 2) {
                std::cerr << "Error: invalid mask file '" << mask_file
                    << "', expected a path to a pnm image file.\n";
                return EXIT_FAILURE;
            }

            // get the lowercase extension
            std::string extension = parts[parts.size() - 1];
            std::for_each(extension.begin(), extension.end(), [](char & c){
                c = ::tolower(c);
            });
            const char* ext = extension.c_str();

            // read input file if valid
            if (strcmp(ext, "pnm") == 0) {
                boost::gil::read_and_convert_image(mask_file, mask_img, boost::gil::pnm_tag());
            } else {
                std::cerr << "Error: invalid mask file extension '" << extension 
                    << "', expected pnm.\n";
                return EXIT_FAILURE;
            }

            valid_mask_provided = true;
        }

        if (vm.count("outfile")) {
            // split the filename to check the extension
            std::string out_file = vm["outfile"].as<std::string>();
            std::vector<std::string> parts;
            boost::split(parts, out_file, boost::is_any_of("."));

            if (parts.size() < 2) {
                std::cerr << "Error invalid outfile '" << out_file
                    << "', expected a path with a pnm extension.\n";
                return EXIT_FAILURE;
            }

            // get the lowercase extension
            std::string extension = parts[parts.size() - 1];
            std::for_each(extension.begin(), extension.end(), [](char & c){
                c = ::tolower(c);
            });
            const char* ext = extension.c_str();

            // validate extension
            if (strcmp(ext, "pnm") != 0) {
                std::cerr << "Error: invalid outfile extension '" << extension 
                    << "', expected pnm.\n";
                return EXIT_FAILURE;
            }

            out_file_ = out_file;
        }
    } catch (const po::error &ex) {
        std::cerr << ex.what() << '\n';
        return EXIT_FAILURE;
    }

    if (valid_mask_provided) {
        venation_.mask(mask_img);
    } else {
        venation_.configure(width, height);
    }

    return EXIT_SUCCESS;
}

void App::scale_to_fit(int window_width, int window_height) {
    venation_.scale_to_fit(window_width, window_height);
}

void App::setup() {
    venation_.setup();

    if (timeout_ > 0.0) {
        start_ = std::chrono::system_clock::now();
    }
}

void App::check_timeout() {
    if (!running_ || timeout_ == 0.0) {
        return;
    }

    // get current elapsed running time
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
