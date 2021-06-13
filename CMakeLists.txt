# setup
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(branching_growth LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_MODULE_PATH /)
set(CMAKE_BUILD_TYPE "Release")

# dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(CGAL REQUIRED)

if (APPLE)
    find_package(glfw3 REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
	add_compile_options("-frounding-math")
endif()

# application
add_executable(app src/main.cpp src/app.hpp src/app.cpp src/node.hpp 
    src/node.cpp src/venation.hpp src/venation.cpp)

target_include_directories(app PUBLIC ${GLFW_INCLUDE_DIRS} 
    ${OPENGL_INCLUDE_DIR} ${CGAL_INCLUDE_DIRS} ./lib)

target_link_libraries(app OpenGL::GL GLEW::GLEW ${GLFW_LIBRARIES} CGAL::CGAL)

