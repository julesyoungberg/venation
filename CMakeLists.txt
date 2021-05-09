# setup
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(branching_growth LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# dependencies
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# application
add_executable(app src/main.cpp src/scene.cpp src/scene.hpp)

target_include_directories(app PUBLIC ${GLUT_INCLUDE_DIR} 
    ${OPENGL_INCLUDE_DIR})

target_link_libraries(app ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

