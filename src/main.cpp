#include <GLUT/glut.h>

#include "scene.hpp"

Scene* scene;

void display() {
    scene->display();
}

int main(int argc, char** argv) {
    // initialize app
    glutInit(&argc, argv);
    auto scene = Scene();

    // create window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(512, 512);
    glutCreateWindow(argv[0]);
    
    // animate
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}

