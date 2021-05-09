#include <GLUT/glut.h>

// main animation loop
void display() {
    glClearColor(0.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    // initialize GLUT
    glutInit(&argc, argv);

    // create window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(512, 512);
    glutCreateWindow(argv[0]);
    
    // animate
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}

