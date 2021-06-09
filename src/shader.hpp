#pragma once

#include <string>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

namespace shader {

unsigned int compile_string(const char**, GLenum);

unsigned int compile_file(std::string, GLenum);

unsigned int create_program(unsigned int vert, unsigned int frag);

};

