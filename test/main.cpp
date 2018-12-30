
#include <visuals.hpp>

#include <iostream>

#include <GL/glut.h>

void resize(int width, int height);

void inspect(unsigned char key, int mousex, int mousey);

int main(int argc, char * argv[])
{
    solar_system::setup();
    
    solar_system::alloc();

    glutInit(&argc, argv);

    glutInitWindowPosition(50, 50);

    glutInitWindowSize(1280, 720);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Solar system visualization");

    glutDisplayFunc(solar_system::render);

    glutIdleFunc(solar_system::update);

    glutReshapeFunc(resize);

    glutMainLoop();

    solar_system::dealloc();

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

    gluPerspective(45, ratio, 0, 300);

    glMatrixMode(GL_MODELVIEW);
}

void inspect(unsigned char key, int mousex, int mousey)
{
    // code
}
