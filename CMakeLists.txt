# setup
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(branching_growth LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_MODULE_PATH /)

# dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(CGAL REQUIRED)
find_package(Boost 1.75 REQUIRED COMPONENTS program_options)

if (APPLE)
    find_package(glfw3 3.3 REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
	add_compile_options("-frounding-math")
endif()

# build growth library
add_library(growth lib/growth/node.cpp lib/growth/venation.cpp)
target_include_directories(growth PUBLIC include ${GLFW_INCLUDE_DIRS} 
    ${OPENGL_INCLUDE_DIR} ${CGAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})

# build application
add_executable(venation app/main.cpp app/app.cpp)
target_link_libraries(venation growth)
target_include_directories(venation PUBLIC include ${GLFW_INCLUDE_DIRS} 
    ${OPENGL_INCLUDE_DIR} ${CGAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})
if (APPLE)
    target_link_libraries(venation OpenGL::GL GLEW::GLEW glfw
        CGAL::CGAL ${Boost_LIBRARIES})
else()
    target_link_libraries(venation OpenGL::GL GLEW::GLEW ${GLFW_LIBRARIES} 
        CGAL::CGAL ${Boost_LIBRARIES})
endif()

# Install the hello and goodbye programs.
install(TARGETS venation DESTINATION bin)

# Install the demo script.
install(PROGRAMS demo DESTINATION bin)
