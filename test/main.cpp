
#include <visuals.hpp>

#include <iostream>

#include <GL/glut.h>

void resize(int width, int height);

int main(int argc, char * argv[])
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(1280, 720);

    glutInitWindowPosition(50, 50);

    glutCreateWindow("Solar system visualization");

    solar_system::setup();

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(solar_system::render);

    glutIdleFunc(solar_system::update);

	glutKeyboardFunc(solar_system::input);

    glutReshapeFunc(resize);

    glutMainLoop();

    return 0;
}

void resize(int width, int height)
{
    if (!height)
        height = 1;

    const float ratio = static_cast<float>(width) / static_cast<float>(height);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glViewport(0, 0, width, height);

    gluPerspective(45, ratio, 20, 300);

    glMatrixMode(GL_MODELVIEW);
}
