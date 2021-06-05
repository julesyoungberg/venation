#include <iostream>

#include "shader.hpp"

shader::shader(std::string filename) {
    std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind("\\"));
    std::cout << dir_path << "\n";
}

