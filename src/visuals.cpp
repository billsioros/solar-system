
#include <visuals.hpp>

#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

#include <GL/glut.h>

#define INIT_FACE_ONE(one, ss)              \
do                                          \
{                                           \
    ss >> one[0];                           \
    ss >> one[1];                           \
                                            \
    ss.ignore(2);                           \
} while (false)                             \

#define INIT_FACE_ALL(face, ss)             \
do                                          \
{                                           \
    INIT_FACE_ONE(face.fst, ss);            \
                                            \
    INIT_FACE_ONE(face.snd, ss);            \
                                            \
    INIT_FACE_ONE(face.thd, ss);            \
} while (false)                             \

#define INIT_POINT(point, ss)               \
do                                          \
{                                           \
    ss >> point.x;                          \
    ss >> point.y;                          \
    ss >> point.z;                          \
} while (false)                             \

solar_system::Planet::Planet
(
    const std::string& wavefront,
    const utility::Point& position,
    double scale,
    const utility::Color& color
)
: position(position), scale(scale), color(color)
{
    std::ifstream ifs(wavefront);

    if (!ifs.is_open())
    {
        std::perror(("Failed to open file " + wavefront).c_str());
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(ifs, line))
    {
        std::stringstream ss(line);

        if (line[0] == 'f')
        {
            Face face;

            INIT_FACE_ALL(face, ss);

            faces.push_back(face);
        }
        else if (line[0] == 'v')
        {
            utility::Point point;

            INIT_POINT(point, ss);

            if (line[1] != 'n')
                vertices.push_back(point);
            else
                normals.push_back(point);
        }
        else
        {
            std::cerr << "<WRN>: Ignoring line " + line << std::endl;
        }
    }

    if (ifs.bad())
    {
        std::perror(("Failed to parse file " + wavefront).c_str());
        std::exit(EXIT_FAILURE);
    }
}

void solar_system::Planet::render()
{
    glPushMatrix();

    glBegin(GL_TRIANGLES);

    for (const auto& face : faces)
    {
        auto& fstn = vertices[face.fst[0] - 1];
        auto& sndn = vertices[face.snd[0] - 1];
        auto& thdn = vertices[face.thd[0] - 1];

        glVertex3f(fstn.x, fstn.y, fstn.z);
        glVertex3f(sndn.x, sndn.y, sndn.z);
        glVertex3f(thdn.x, thdn.y, thdn.z);
    }

    glEnd();

    glPopMatrix();
}

solar_system::Star::Star(const utility::Point& position, std::size_t radius, const utility::Color& color)
:
position(position), radius(radius), color(color)
{
}

void solar_system::Star::render()
{

}

void solar_system::setup()
{
	ReadFile(&md);

	//Parameter handling
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);  //renders a fragment if its z value is less or equal of the stored value
	glClearDepth(1);

	// polygon rendering mode
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

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

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	//01
	glFrontFace(GL_CCW);


	// Black background
	glClearColor(0.0f,0.0f,0.0f,1.0f);

}

void solar_system::render()
{
  //CLEARS FRAME BUFFER ie COLOR BUFFER& DEPTH BUFFER (1.0)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clean up the colour of the window
													   // and the depth buffer
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();


  glTranslatef(0,0,-100);
  glTranslatef(tx,0.0,0.0);
  glRotatef(rotx,1,0,0);



  //(01)
  glColor3f(0.3, 0.2, 0.9);                            // Set drawing colour
  DisplayModel(md);

  //(02)
  //glColor3f(0.8, 0.1, 0.1);
  //glTranslatef(-20.0,0.0,0.0);
  //keimeno("Dokimastiko keimeno",0.05f);

  //(03)
  //glColor3f(red, green, blue);                            // Set drawing colour
  //glutSolidTeapot(20.0);


  glutSwapBuffers();             // All drawing commands applied to the
                                 // hidden buffer, so now, bring forward
                                 // the hidden buffer and hide the visible one
}

void solar_system::update()
{
    // code
}

void solar_system::keyboard(unsigned char key, int mousex, int mousey)
{
    // code
}
