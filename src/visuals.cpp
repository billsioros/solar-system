
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

// Utilities
#define RAND01 (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX))

#define RANGE(min, max) ((max - min) * RAND01 + min)

// Parameters
#define __WAVEFRONT_PATH__ "/home/massiva/Documents/Courses/Graphics/data/planet.obj"
// #define __WAVEFRONT_PATH__ "C:/Users/johnf/Source/Repos/solar-system/data/planet.obj"

#define STAR_COUNT          (400UL)

#define STAR_POSITION       spherical(RANGE(250.0f, 200.0f), RANGE(-45.0f, 45.0f), RANGE(-45.0f, 45.0f), SUN_POSITION)
#define STAR_SIZE           (0.5f)
#define STAR_COLOR          { 1.0f, 0.964f, 0.756f, 1.0f }

#define SUN_POSITION        { 0.0f, 0.0f, -100.0f }
#define SUN_SIZE            (20.0f)
#define SUN_COLOR           { 1.0f, 0.647f, 0.078f, 1.0f }

#define RING_SIZE           (SUN_SIZE * (12.0f / 11.0f))
#define RING_DALPHA         (0.00125f)
#define RING_ALPHA_MIN      (0.05f)
#define RING_ALPHA_MAX      (0.3f)
#define RING_COLOR          { 1.0f, 0.89f, 0.078f, RING_ALPHA_MIN }

#define EARTH_SIZE          (0.00625f)
#define EARTH_COLOR         { 0.0f, 0.435f, 0.639f, 1.0f }
#define EARTH_THETA         (0.0f)
#define EARTH_DTHETA        (0.01f)
#define EARTH_PHI           (0.0f)
#define EARTH_DPHI          (0.0f)
#define EARTH_DISTANCE      (40.0f)
#define EARTH_POSITION      spherical(EARTH_DISTANCE, EARTH_THETA, EARTH_PHI, SUN_POSITION)

#define MOON_SIZE           (EARTH_SIZE / 2.0f)
#define MOON_COLOR          { 0.219f, 0.219f, 0.219f, 1.0f }
#define MOON_THETA          (45.0f)
#define MOON_DTHETA         (0.0f)
#define MOON_PHI            (0.0f)
#define MOON_DPHI           (0.02f)
#define MOON_DISTANCE       (EARTH_DISTANCE / 4.0f)
#define MOON_POSITION       spherical(MOON_DISTANCE, MOON_THETA, MOON_PHI, EARTH_POSITION)

static bool animate = true;
static float tx = 0, ty = 0, tz = 0;

// Return a point whose cartesian coordinates
// correspond to the spherical coordinates specified
static detail::Vector3 spherical
(
    float distance,
    float theta,
    float phi,
    const detail::Vector3& center
)
{
    return
    {
        distance * std::sin(theta) * std::cos(phi) + center.x,
        distance * std::sin(theta) * std::sin(phi) + center.y,
        distance * std::cos(theta) + center.z
    };
}

// Wavefront struct
detail::Wavefront::Wavefront(const char * wavefront_path)
{
    std::ifstream ifs(wavefront_path);

    if (!ifs.is_open())
    {
        std::perror(("<ERR>: Failed to open file " + std::string(wavefront_path)).c_str());
        std::exit(EXIT_FAILURE);
    }

    std::string line; std::istringstream ss;
    while (std::getline(ifs, line))
    {
        ss.clear(); ss.str(line); ss.ignore(3);

        if (line[0] == 'f')
        {
            Face face;

            char skipped;

            ss >> face.fst[0] >> skipped >> skipped >> face.fst[1];
            ss >> face.snd[0] >> skipped >> skipped >> face.snd[1];
            ss >> face.thd[0] >> skipped >> skipped >> face.thd[1];

            faces.push_back(face);
        }
        else if (line[0] == 'v')
        {
            detail::Vector3 Vector3;
            
            ss >> Vector3.x; ss >> Vector3.y; ss >> Vector3.z;

            if (line[1] != 'n')
                vertices.push_back(Vector3);
            else
                normals.push_back(Vector3);
        }
        else
        {
            std::cerr << "<MSG>: Ignoring line " + line << std::endl;
        }
    }

    if (ifs.bad())
    {
        std::perror(("<ERR>: Failed to parse file " + std::string(wavefront_path)).c_str());
        std::exit(EXIT_FAILURE);
    }
}

void detail::Wavefront::render() const
{
    glBegin(GL_TRIANGLES);

    for (const auto& face : faces)
    {
        const auto& v1 = vertices[face.fst[0] - 1];
        const auto& v2 = vertices[face.snd[0] - 1];
        const auto& v3 = vertices[face.thd[0] - 1];

        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }

    glEnd();
}

// Object class
// Save all the necessary info
// to render the star as a simple sphere
solar_system::Object::Object(const detail::Vector3& position, float size, const detail::Color& color)
:
position(position), size(size), color(color)
{
}

void solar_system::Object::render() const
{
    glColor4f(color.red, color.green, color.blue, color.alpha);

    glPushMatrix();

    glTranslatef(position.x, position.y, position.z);

    glutSolidSphere(size, 5, 5);

    glPopMatrix();
}

void solar_system::Object::update() {}

// Sun class
// The sun is a special case of a star that radiates
solar_system::Sun::Sun
(
    const detail::Vector3& position,
    float size,
    const detail::Color& color,
    const detail::Color& ring_color,
    float dalpha
)
:
Object(position, size, color),
ring_color(ring_color),
dalpha(dalpha)
{
}

void solar_system::Sun::render() const
{
    glPushMatrix();

    glTranslatef(position.x, position.y, position.z);

    glColor4f(color.red, color.green, color.blue, color.alpha);

    glutSolidSphere(size, 50, 50);

    glColor4f(ring_color.red, ring_color.green, ring_color.blue, ring_color.alpha);

    glutSolidSphere(RING_SIZE, 50, 50);

    glPopMatrix();
}

void solar_system::Sun::update()
{    
    ring_color.alpha += dalpha;

    if (ring_color.alpha < RING_ALPHA_MIN)
    {
        ring_color.alpha = RING_ALPHA_MIN; dalpha *= -1.0f;
    }
    else if (ring_color.alpha > RING_ALPHA_MAX)
    {
        ring_color.alpha = RING_ALPHA_MAX; dalpha *= -1.0f;
    }
}

// Planet class
// Parse the specified wavefront file
// and save all the information necessary
// to render it as a 3d object
const detail::Wavefront * solar_system::Planet::wavefront = nullptr;

solar_system::Planet::Planet
(
    const detail::Vector3& position,
    float size,
    const detail::Color& color,
    const Object * rotating,
    float theta,
    float dtheta,
    float phi,
    float dphi,
    float distance
)
:
Object(position, size, color),
rotating(rotating),
theta(theta),
dtheta(dtheta),
phi(phi),
dphi(dphi),
distance(distance)
{
}

void solar_system::Planet::render() const
{
    glColor4f(color.red, color.green, color.blue, color.alpha);

    glPushMatrix();

    glTranslatef(position.x, position.y, position.z);

    glScalef(size, size, size);

    wavefront->render();

    glPopMatrix();
}

void solar_system::Planet::update()
{
    theta += (theta < 360.0f ? dtheta : -360.0f);

    phi += (phi < 360.0f ? dphi : -360.0f);

    position = spherical(distance, theta, phi, rotating->position);
}

// Allocate the necessary resources
static std::vector<solar_system::Object *> objects;

void solar_system::alloc()
{
    Planet::wavefront = new detail::Wavefront(__WAVEFRONT_PATH__);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (std::size_t s = 0UL; s < STAR_COUNT; s++)
        objects.push_back(new Star(STAR_POSITION, STAR_SIZE, STAR_COLOR));

    Object * sun = new Sun(SUN_POSITION, SUN_SIZE, SUN_COLOR, RING_COLOR, RING_DALPHA);

    Object * earth = new Planet
    (
        EARTH_POSITION,
        EARTH_SIZE,
        EARTH_COLOR,
        sun,
        EARTH_THETA,
        EARTH_DTHETA,
        EARTH_PHI,
        EARTH_DPHI,
        EARTH_DISTANCE
    );

    Object * moon = new Planet
    (
        MOON_POSITION,
        MOON_SIZE,
        MOON_COLOR,
        earth,
        MOON_THETA,
        MOON_DTHETA,
        MOON_PHI,
        MOON_DPHI,
        MOON_DISTANCE
    );

    objects.push_back(moon);

    objects.push_back(earth);

    objects.push_back(sun);
}

void solar_system::dealloc()
{
    delete Planet::wavefront;

    for (const auto body_ptr : objects)
        delete body_ptr;
}

void solar_system::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -100.f);
	glRotatef(tx, 1, 0, 0);
	glRotatef(ty, 0, 1, 0);
	glRotatef(tz, 0, 0, 1);
	glTranslatef(0.0f, 0.0f, 100.f);

    for (const auto body_ptr : objects)
    {
        body_ptr->render();
    }

    // Draw axes
	glColor3f(0.6, 0.6, 0.6);

	glBegin(GL_LINES);

    glVertex3f(0.0,  0.0,  -100.0);
    glVertex3f(50.0, 0.0,  -100.0);
    glVertex3f(0.0,  0.0,  -100.0);
    glVertex3f(0.0,  0.0,    50.0);
    glVertex3f(0.0,  0.0,  -100.0);
    glVertex3f(0.0,  50.0, -100.0);

	glEnd();

    glutSwapBuffers();
}

void solar_system::update()
{
	if(animate)
		for (auto body_ptr : objects)
			body_ptr->update();

    glutPostRedisplay();
}

void solar_system::inspect(unsigned char key, int, int)
{
	switch (key)
	{
	case 'q': std::exit(EXIT_SUCCESS); break;
	case 'r': tx = 0; ty = 0; tz = 0; break;
	case 'p': animate = !animate; break;
	case 'a': tx += 1.0f;  break;
	case 'z': tx -= 1.0f;  break;
	case 's': ty += 1.0f;  break;
	case 'x': ty -= 1.0f;  break;
	case 'd': tz += 1.0f;  break;
	case 'c': tz -= 1.0f;  break;
	default: break;
	}
}