
#include <visuals.hpp>

#include <vector>
#include <string>
#include <cstdint>

#include <sstream>
#include <fstream>
#include <iostream>

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <GL/glut.h>

#define __WAVEFRONT_OBJ__ "/home/massiva/Documents/Courses/Graphics/data/planet.obj"

double utility::rand(double min, double max)
{
    return ((max - min) * (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX)) + min);
}

int utility::rand(int min, int max)
{
    return ((std::rand() %  (max - min + 1)) + min);
}

utility::Point utility::spherical2cartesian(double radius, double theta, double phi)
{
    return
    {
        radius * std::sin(theta) * std::cos(phi),
        radius * std::sin(theta) * std::sin(phi),
        radius * std::cos(theta)
    };
}

#define INIT_POINT(line, point)                         \
do                                                      \
{                                                       \
    std::istringstream ss(line);                        \
                                                        \
    ss >> point.x; ss >> point.y; ss >> point.z;        \
} while (false)                                         \

#define INIT_FACE(line, face)                           \
do                                                      \
{                                                       \
    std::istringstream ss(line);                        \
                                                        \
    ss >> face.fst[0]; ss >> face.fst[1];               \
    ss >> face.snd[0]; ss >> face.snd[1];               \
    ss >> face.thd[0]; ss >> face.thd[1];               \
                                                        \
} while (false)                                         \

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
        std::perror(("<ERR>: Failed to open file " + wavefront).c_str());
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(ifs, line))
    {
        if (line[0] == 'f')
        {
            Face face; INIT_FACE(line, face); faces.push_back(face);
        }
        else if (line[0] == 'v')
        {
            utility::Point point; INIT_POINT(line, point);

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
        std::perror(("<ERR>: Failed to parse file " + wavefront).c_str());
        std::exit(EXIT_FAILURE);
    }
}

void solar_system::Planet::render()
{
    glColor4b(color.red, color.green, color.blue, color.alpha);

    glPushMatrix();

    glLoadIdentity();

    glTranslated(position.x, position.y, position.z);

    glScaled(scale, scale, scale);

    glBegin(GL_TRIANGLES);

    for (const Face& face : faces)
    {
        const utility::Point& fstn = vertices[face.fst[0] - 1];
        const utility::Point& sndn = vertices[face.snd[0] - 1];
        const utility::Point& thdn = vertices[face.thd[0] - 1];

        glVertex3f(fstn.x, fstn.y, fstn.z);
        glVertex3f(sndn.x, sndn.y, sndn.z);
        glVertex3f(thdn.x, thdn.y, thdn.z);
    }

    glEnd();

    glPopMatrix();
}

solar_system::Star::Star()
:
position({ utility::rand(-100, 100), utility::rand(-100, 100), utility::rand(-100, 100)}),
radius(utility::rand(10.0, 20.0)),
color({ 255, 255, 0, utility::rand(128, 255) })
{
}

solar_system::Star::Star(const utility::Point& position, std::size_t radius, const utility::Color& color)
:
position(position), radius(radius), color(color)
{
}

void solar_system::Star::render()
{
    glColor4b(color.red, color.green, color.blue, color.alpha);

    glPushMatrix();

    glLoadIdentity();

    glTranslated(position.x, position.y, position.z);

    glutSolidSphere(radius, 20, 20);

    glPopMatrix();
}

static const std::size_t STAR_COUNT = 100UL;

static solar_system::Star * sun;
static solar_system::Star * ring;
static solar_system::Star * stars;

static solar_system::Planet * earth;
static solar_system::Planet * moon;

void solar_system::setup()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sun = new Star
    (
        { 0.0, 0.0, 0.0 },
        100UL,
        { 255, 255, 0, 255 }
    );

    ring = new Star
    (
        { 0.0, 0.0, 0.0 },
        150UL,
        { 255, 255, 0, 127 }
    );

	stars = new Star[STAR_COUNT];

    earth = new Planet
    (
        __WAVEFRONT_OBJ__,
        utility::spherical2cartesian(100.0, 45.0, 0.0),
        1.0,
        { 0, 255, 255, 255 }
    );

    moon = new Planet
    (
        __WAVEFRONT_OBJ__,
        utility::spherical2cartesian(300.0, 60.0, 00),
        0.5,
        { 51, 51, 51, 255 }
    );

	glShadeModel(GL_FLAT);

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
