# setup
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(branching_growth LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_MODULE_PATH /)
set(CMAKE_BUILD_TYPE "Release")
set(GL_SILENCE_DEPRECATION TRUE)

# dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
# find_package(PkgConfig REQUIRED)
# pkg_search_module(GLFW REQUIRED glfw3)
find_package(glfw3 REQUIRED)
find_package(cgal REQUIRED)

# application
add_executable(app src/main.cpp src/app.cpp src/app.hpp src/shader.cpp src/shader.hpp)

target_include_directories(app PUBLIC ${GLFW_INCLUDE_DIRS} 
    ${OPENGL_INCLUDE_DIR} ${CGAL_INCLUDE_DIRS} ./lib)

target_link_libraries(app GLEW::GLEW OpenGL::GL ${GLFW_LIBRARIES} CGAL::CGAL)

