
#include <visuals.hpp>

#include <iostream>

#include <GL/glut.h>

void resize(int w, int h);

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <wavefront-obj-file>" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // initialize GLUT library state
    glutInit(&argc, argv);

    // Set up the display using the GLUT functions to
    // get rid of the window setup details:
    // - Use true RGB colour mode ( and transparency )
    // - Enable double buffering for faster window update
    // - Allocate a Depth-Buffer in the system memory or
    //   in the video memory if 3D acceleration available
                        //RGBA//DEPTH BUFFER//DOUBLE BUFFER//
    glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);


    // Define the main window size and initial position
    // ( upper left corner, boundaries included )
    glutInitWindowSize(480,480);
    glutInitWindowPosition(50,50);

    // Create and label the main window
    glutCreateWindow("Course5");

    // Configure various properties of the OpenGL rendering context
    //Set up light source
    GLfloat light_position[] = { 0.0, 30.0, 50.0, 0.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, light_position);

    GLfloat ambientLight[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat diffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };


    glLightfv( GL_LIGHT0, GL_AMBIENT, ambientLight );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight );


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Callbacks for the GL and GLUT events:

    // The rendering function
    glutDisplayFunc(solar_system::render);
    glutIdleFunc(solar_system::update);
    glutKeyboardFunc(solar_system::keyboard);
    glutReshapeFunc(resize);

    // attach the menu to the right button
    glutAttachMenu(GLUT_RIGHT_BUTTON);


    //Enter main event handling loop
    glutMainLoop();

    return 0;
}

void resize(int w, int h)
{
  // define the visible area of the window ( in pixels )
  if (h==0) h=1;
  glViewport(0,0,w,h);

  // Setup viewing volume

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(60.0, (float)w/(float)h, 1.0, 500.0);
}
