#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "app.hpp"

namespace po = boost::program_options;

static App app;

void display(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
    app.draw();
    glfwSwapBuffers(window);
}

int run_app(const char* name) {
    // initialize app
    if (!glfwInit()) { return EXIT_FAILURE; }
    glfwSwapInterval(1);

    // create window
    GLFWwindow* window = glfwCreateWindow(512, 512, name, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    // animate
    while (!glfwWindowShouldClose(window)) {
        app.update();
        display(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}

int main(int argc, const char* argv[]) {
    try {
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "produce help message")
            ("width", po::value<unsigned int>()->default_value(512), 
                "Simulation width in pixels. Defaults to 512.")
            ("height", po::value<unsigned int>()->default_value(512), 
                "Simulation height in pixels. Defaults to 512.")
            ("num-attractors", po::value<unsigned int>()->default_value(5000),
                "Number of random attractors to generate. Defaults to 5000.")
            ("seeds", po::value<std::string>(),
                "A list of 2D points to start growing from. Input should be of "
                " the form: (x1,y2),(x2,y2). Defaults to (width/2, height/2).")
            ("mode", po::value<std::string>()->default_value("open"),
                "Growth mode, 'open' or 'closed' venation styles. "
                "Defaults to 'open'.")
            ("mask", po::value<std::string>(),
                "A path to an image that will be used to mask the attractors. "
                "i.e. generated attractors will only be kept for the "
                "simulation if the corresponding pixel in the image is bright "
                "enough. ")
            ("outfile", po::value<std::string>(), 
                "An image path to store the result at.")
            ("timeout", po::value<unsigned int>()->default_value(500),
                "A time limit in seconds after which the simulation result "
                "will be saved to the output file and the program will "
                "terminate. Defaults to 500.")
            ("growth-radius", po::value<long double>()->default_value(0.1),
                "The maximum distance an attractor can be from a growth node "
                "and still influence it (relative to normalized points)."
                "Defaults to 0.1.")
            ("growth-rate", po::value<long double>()->default_value(0.002),
                "The size of the step taken at each growth step (relative to "
                "normalized points). Defaults to 0.002.")
            ("consume-radius", po::value<long double>()->default_value(0.0005),
                "The distance between an attractor and node at which point "
                "the attractor is considered consumed and removed "
                "(relative to normalized points). Defaults to 0.0005.");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Venation Pattern Generator\n\n"
                << "This program simulates venation growth using a "
                << "space colonization algorithm.\n\n";
            std::cout << desc << '\n';
            return 1;
        }

        if (vm.count("width")) {
            std::cout << "width: " << vm["width"].as<unsigned int>() << '\n';
        }

        if (vm.count("height")) {
            std::cout << "height: " << vm["height"].as<unsigned int>() << '\n';
        }
    } catch (const po::error &ex) {
        std::cerr << ex.what() << '\n';
    }

    app.setup();

    return run_app(argv[0]);
}
