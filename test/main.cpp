/*
Simplistic solar system implemented in OpenGL

* sdi1500144 - Vasileios Sioros
* sdi1500176 - Ioannis Heilaris
*/

#include <visuals.hpp>

#include <GL/glut.h>

void resize(int width, int height);

int main(int argc, char * argv[])
{
    // Initialize glut
    glutInit(&argc, argv);

    // Initialize the display mode:
    // (1) Use rgba coloring
    // (2) Use double buffering to avoid distortions
    // (3) Allocate a depth buffer for future depth testing
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    // Specify the window's dimensions
    glutInitWindowSize(1280, 720);

    // Specify the window's location in pixel space
    glutInitWindowPosition(50, 50);

    // Specify the window's title
    glutCreateWindow("Solar system visualization");

    // Set up the solar system
    solar_system::setup();

    // Enable depth testing in order to properly draw each entity
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    // Enable blending so that the ring of the sun appears transparent
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Register the display, idle and keyboard input callbacks
    glutDisplayFunc(solar_system::render);

    glutIdleFunc(solar_system::update);

	glutKeyboardFunc(solar_system::input);

    // Register a general resizing function as a reshape callback
    glutReshapeFunc(resize);

    // Enter the main event loop
    glutMainLoop();

    return 0;
}

// Firstly make sure that the window's height never becomes zero,
// as it results in the window behaving weirdly and a division by zero
// Switch to projection mode and load the identity matrix to reset any changes
// Lastly, set the whole window as the viewport and specify the perspective
// projection's parameters
void resize(int width, int height)
{
    if (!height)
        height = 1;

    const float ratio = static_cast<float>(width) / static_cast<float>(height);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glViewport(0, 0, width, height);

    gluPerspective(60, ratio, 20, 500);

    glMatrixMode(GL_MODELVIEW);
}
