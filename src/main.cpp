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
    std::string description = "Venation Pattern Generator\n\
\n\
This program simulates venation growth using a space colonization algorithm.\n\
\n\
Options:";

    try {
        po::options_description desc(description);
        desc.add_options()
            ("help,h", "produce help message")
            ("width", po::value<unsigned int>()->default_value(512), "Simulation width in pixels.")
            ("height", po::value<unsigned int>()->default_value(512), "Simulation height in pixels.");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
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
