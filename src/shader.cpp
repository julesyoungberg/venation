#include <cstdlib>
#include <iostream>
#include <fstream>

#include "shader.hpp"

/**
 * Reads a shader file into a string.
 */
std::string read(const std::string& path) {
    std::string source = "";
    std::ifstream fs(path, std::ios::in);

    if (!fs.is_open()) {
        std::cout << "ERROR: could not read shader: " << path << "\n";
        std::exit(1);
    }

    std::string line = "";
    while (!fs.eof()) {
        std::getline(fs, line);
        source.append(line + "\n");
    }

    fs.close();

    return source;
}

/**
 * Compiles a GLSL shader string.
 */
unsigned int shader::compile_string(const char** src, GLenum type) {
    std::cout << *src << std::endl;
    // compile the shader code
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, src, NULL);
    glCompileShader(shader);

    // handle errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cout << "ERROR: could not compile shader.\n" << log << std::endl;
        std::exit(1);
    }

    return shader;
}

/**
 * Compiles a GLSL shader file.
 */
unsigned int shader::compile_file(const std::string filename, GLenum type) {
    // Read shaders from the ./src/shaders directory
    const std::string file_path = __FILE__;
    const std::string dir_path = file_path.substr(0, file_path.rfind("/"));
    const std::string shader_path = dir_path + "/shaders/" + filename;
    std::cout << "shader: " << shader_path << "\n";
    const std::string source = read(shader_path);
    const char* src = source.c_str();
    return shader::compile_string(&src, type);
}

/**
 * Creates a Shader program.
 */
unsigned int shader::create_program(unsigned int vert, unsigned int frag) {
    // create program
    unsigned int program = glCreateProgram();

    // attach shaders
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    
    // handle errors
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        std::cout << "ERROR: could not link shader program." << log << std::endl;
        std::exit(1);
    }

    return program;
}
