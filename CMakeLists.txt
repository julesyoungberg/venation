# setup
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(branching-growth LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(APPLE)
    LINK_DIRECTORIES(/usr/local/Cellar)
endif()

# dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

# application
add_executable(app src/app.cpp)

target_include_directories(app PUBLIC ${GLFW_INCLUDE_DIRS} 
    ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

target_link_libraries(app ${GLFW_LIBRARIES} ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES})

