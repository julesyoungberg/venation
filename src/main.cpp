#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "app.hpp"
#include "venation.hpp"

namespace po = boost::program_options;

int run_app(App& app, const char* name) {
    // initialize app
    if (!glfwInit()) { return EXIT_FAILURE; }
    glfwSwapInterval(1);

    // create window
    GLFWwindow* window = glfwCreateWindow(app.width(), app.height(), name, 
        nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    // animate
    while (!glfwWindowShouldClose(window)) {
        app.update();
        glfwMakeContextCurrent(window);
        app.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}

void erase_all(std::string& str, char c) {
    if (str.find(c) != std::string::npos) {
        str.erase(std::remove(str.begin(), str.end(), c), str.end());
    }
}

int main(int argc, const char* argv[]) {
    App app;
    
    // app defaults
    unsigned int width = 512;
    unsigned int height = 512;
    unsigned int num_attractors = 5000;
    std::string mode("open");
    unsigned int timeout = 500;
    long double growth_radius = 0.1;
    long double growth_rate = 0.002;
    long double consume_radius = 0.0005;
    std::vector<venation::point2> seeds;

    // parse command line options
    try {
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "produce help message")
            ("width", po::value<unsigned int>(), 
                "Simulation width in pixels. Defaults to 512.")
            ("height", po::value<unsigned int>(), 
                "Simulation height in pixels. Defaults to 512.")
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
                "terminate. Defaults to 500.")
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
            ("mask", po::value<std::string>(),
                "A path to an image that will be used to mask the attractors. "
                "i.e. generated attractors will only be kept for the "
                "simulation if the corresponding pixel in the image is bright "
                "enough. ")
            ("outfile", po::value<std::string>(), 
                "An image path to store the result at.");

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
            num_attractors = vm["num-attractors"].as<unsigned int>();
        }

        if (vm.count("seeds")) {
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
        }

        if (vm.count("mode")) {
            mode = vm["mode"].as<std::string>();
        }

        if (vm.count("timeout")) {
            timeout = vm["timeout"].as<unsigned int>();
        }

        if (vm.count("growth-radius")) {
            growth_radius = vm["growth-radius"].as<long double>();
        }

        if (vm.count("growth-rate")) {
            growth_rate = vm["growth-rate"].as<long double>();
        }

        if (vm.count("consume-radius")) {
            consume_radius = vm["consume-radius"].as<long double>();
        }
    } catch (const po::error &ex) {
        std::cerr << ex.what() << '\n';
        return EXIT_FAILURE;
    }

    // configure, setup, and run the app
    app.configure(width, height, num_attractors, mode, timeout, growth_radius, 
        growth_rate, consume_radius, seeds);

    app.setup();

    return run_app(app, argv[0]);
}
