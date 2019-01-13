/*
Simplistic solar system visualization in OpenGL by

sdi1500144 - Vasileios Sioros
sdi1500176 - Ioannis Heilaris
*/

#include <visuals.hpp>

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <memory>

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <GL/glut.h>

// A function returning a random floating point number
// in the range [min, max]
#define random(min, max)                                                \
(                                                                       \
    (max - min) *                                                       \
    (                                                                   \
        static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)  \
    ) + min                                                             \
)                                                                       \

// The path to the 3D model
#define __WAVEFRONT_PATH__ "/home/massiva/Documents/Courses/Graphics/data/planet.obj"
// #define __WAVEFRONT_PATH__ "C:/Users/johnf/Source/Repos/solar-system/data/planet.obj"

// After a lot of experimentation, we ended up using the following settings

// Star info:
#define STAR_COUNT          (400UL)

#define STAR_POSITION       spherical(random(200.0f, 250.0f), random(-45.0f, 45.0f), random(-45.0f, 45.0f), SUN_POSITION)
#define STAR_SIZE           (0.5f)
#define STAR_COLOR          { 1.0f, 0.964f, 0.756f, 1.0f }

// Sun info:
#define SUN_POSITION        { 0.0f, 0.0f, -100.0f }
#define SUN_SIZE            (20.0f)
#define SUN_COLOR           { 1.0f, 0.647f, 0.078f, 1.0f }

// Ring info:
#define RING_SIZE           (SUN_SIZE * (12.0f / 11.0f))
#define RING_DALPHA         (0.00125f)
#define RING_ALPHA_MIN      (0.05f)
#define RING_ALPHA_MAX      (0.3f)
#define RING_COLOR          { 1.0f, 0.89f, 0.078f, RING_ALPHA_MIN }

// Earth Info:
#define EARTH_SIZE          (0.00625f)
#define EARTH_COLOR         { 0.0f, 0.435f, 0.639f, 1.0f }
#define EARTH_THETA         (0.0f)
#define EARTH_DTHETA        (0.0025f)
#define EARTH_PHI           (0.0f)
#define EARTH_DPHI          (0.0f)
#define EARTH_DISTANCE      (40.0f)
#define EARTH_POSITION      spherical(EARTH_DISTANCE, EARTH_THETA, EARTH_PHI, SUN_POSITION)

// Moon Info:
#define MOON_SIZE           (EARTH_SIZE / 2.0f)
#define MOON_COLOR          { 0.219f, 0.219f, 0.219f, 1.0f }
#define MOON_THETA          (0.0f)
#define MOON_DTHETA         (0.01f)
#define MOON_PHI            (0.0f)
#define MOON_DPHI           (0.0f)
#define MOON_DISTANCE       (EARTH_DISTANCE / 4.0f)
#define MOON_POSITION       spherical(MOON_DISTANCE, MOON_THETA, MOON_PHI, EARTH_POSITION)

// An unnamed struct that holds all the info having to do with our scene
static struct
{
    bool animate;
    float tx, ty, tz;
    std::vector<std::unique_ptr<solar_system::Entity>> objects;
} scene =
{
    .animate = true,
    .tx = 0.0f, .ty = 0.0f, .tz = 0.0f,
    {}
};

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

// Wavefront class
// Load the specified wavefront 3D model into memory
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

// Render the wavefront 3D object
void detail::Wavefront::render() const
{
    glBegin(GL_TRIANGLES);

    for (const auto& face : faces)
    {
        const auto& v1 = vertices[face.fst[0UL] - 1UL];
        const auto& v2 = vertices[face.snd[0UL] - 1UL];
        const auto& v3 = vertices[face.thd[0UL] - 1UL];

        const auto& n1 = vertices[face.fst[1UL] - 1UL];
        const auto& n2 = vertices[face.snd[1UL] - 1UL];
        const auto& n3 = vertices[face.thd[1UL] - 1UL];

        glNormal3f(n1.x, n1.y, n1.z);
        glVertex3f(v1.x, v1.y, v1.z);

        glNormal3f(n2.x, n2.y, n2.z);
        glVertex3f(v2.x, v2.y, v2.z);

        glNormal3f(n3.x, n3.y, n3.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }

    glEnd();
}

// Star class
solar_system::Star::Star(const detail::Vector3& position, float size, const detail::Color& color)
:
position(position), size(size), color(color)
{
}

// In order to render a star we translate to its position
// and render a glut primitive (solid sphere) with the given color
void solar_system::Star::render() const
{
    glColor4f(color.red, color.green, color.blue, color.alpha);

    glPushMatrix();

    glTranslatef(position.x, position.y, position.z);

    glutSolidSphere(size, 5, 5);

    glPopMatrix();
}

void solar_system::Star::update() {}

// Sun class
solar_system::Sun::Sun
(
    const detail::Vector3& position,
    float size,
    const detail::Color& color,
    const detail::Color& ring_color,
    float dalpha
)
:
Entity(position, size, color),
ring_color(ring_color),
dalpha(dalpha)
{
}

// In order to render the sun, we translate to its position and render two solid spheres
// The first one is of size "size" and the second one is of greater size, specifically
// (12.0f / 11.0f) times greater
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

// Update the alpha of the sun's ring,
// so that it oscillates between RING_ALPHA_MIN and RING_ALPHA_MAX
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

const detail::Wavefront solar_system::Planet::wavefront(__WAVEFRONT_PATH__);

// Planet class
solar_system::Planet::Planet
(
    const detail::Vector3& position,
    float size,
    const detail::Color& color,
    const Entity * rotating,
    float angle,
    float dangle,
    float distance
)
:
Entity(position, size, color),
rotating(rotating),
angle(angle),
dangle(dangle),
distance(distance)
{
}

// Update "angle" so that it oscillates between 0 and 360 degrees
void solar_system::Planet::update()
{
    angle += (angle < 360.0f ? dangle : -360.0f);
}

solar_system::Earth::Earth
(
    const detail::Vector3& position,
    float size,
    const detail::Color& color,
    const Entity * rotating,
    float angle,
    float dangle,
    float distance
)
:
Planet(position, size, color, rotating, angle, dangle, distance)
{
}

// Translate to the earth's position, scale by size in all axes
// and render the 3D model colored a shade of gray
void solar_system::Earth::render() const
{
    glColor4f(color.red, color.green, color.blue, color.alpha);

    glPushMatrix();

    glTranslatef(position.x, position.y, position.z);

    glScalef(size, size, size);

    wavefront.render();

    glPopMatrix();
}

// Update its position according to its current angle
void solar_system::Earth::update()
{
    Planet::update();

    position = spherical(distance, angle, 0.0f, rotating->position);
}

solar_system::Moon::Moon
(
    const detail::Vector3& position,
    float size,
    const detail::Color& color,
    const Entity * rotating,
    float angle,
    float dangle,
    float distance
)
:
Planet(position, size, color, rotating, angle, dangle, distance)
{
}

// Translate to the position of the object being rotated by the moon,
// (i.e. the earth's position) rotate by 90 degrees in the z axis
// and translate back to the original position. This results in the
// moon being shown to rotate earth in the x axis.
// Lastly, translate to the moon's position, scale by size in all axes
// and render the wavefront object colored a shade of blue
void solar_system::Moon::render() const
{
    glColor4f(color.red, color.green, color.blue, color.alpha);

    glPushMatrix();

    glTranslatef(rotating->position.x, rotating->position.y, rotating->position.z);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(-rotating->position.x, -rotating->position.y, -rotating->position.z);

    glTranslatef(position.x, position.y, position.z);

    glScalef(size, size, size);

    wavefront.render();

    glPopMatrix();
}

// Update its position according to its current angle
void solar_system::Moon::update()
{
    Planet::update();

    position = spherical(distance, angle, 0.0f, rotating->position);
}

// Seed the random number generator and allocate all the necessary resources
void solar_system::setup()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (std::size_t s = 0UL; s < STAR_COUNT; s++)
        scene.objects.emplace_back(new Star(STAR_POSITION, STAR_SIZE, STAR_COLOR));

    Entity * sun = new Sun(SUN_POSITION, SUN_SIZE, SUN_COLOR, RING_COLOR, RING_DALPHA);

    Entity * earth = new Earth
    (
        EARTH_POSITION,
        EARTH_SIZE,
        EARTH_COLOR,
        sun,
        EARTH_THETA,
        EARTH_DTHETA,
        EARTH_DISTANCE
    );

    Entity * moon = new Moon
    (
        MOON_POSITION,
        MOON_SIZE,
        MOON_COLOR,
        earth,
        MOON_THETA,
        MOON_DTHETA,
        MOON_DISTANCE
    );

    scene.objects.emplace_back(moon);

    scene.objects.emplace_back(earth);

    scene.objects.emplace_back(sun);
}

// Clear the color and depth buffers and load the identity matrix
// Rotate around the sun in any axis the user wants to
// Draw the 3D axis with the sun as their star point and swap drawing buffers
void solar_system::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -100.f);
	glRotatef(scene.tx, 1.0f, 0.0f, 0.0f);
	glRotatef(scene.ty, 0.0f, 1.0f, 0.0f);
	glRotatef(scene.tz, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, 100.f);

    for (const auto& body_ptr : scene.objects)
        body_ptr->render();

    // Draw axes
	glColor3f(0.6, 0.6, 0.6);

	glBegin(GL_LINES);

    glVertex3f(0.0f,  0.0f,  -100.0f);
    glVertex3f(50.0f, 0.0f,  -100.0f);
    glVertex3f(0.0f,  0.0f,  -100.0f);
    glVertex3f(0.0f,  0.0f,    50.0f);
    glVertex3f(0.0f,  0.0f,  -100.0f);
    glVertex3f(0.0f,  50.0f, -100.0f);

	glEnd();

    glutSwapBuffers();
}

// If the user hasn't paused the program, update the position of every object
void solar_system::update()
{
	if(scene.animate)
		for (auto& body_ptr : scene.objects)
			body_ptr->update();

    glutPostRedisplay();
}

// Input system
// a and z: rotate in x axis
// s and x: rotate in y axis
// d and c: rotate in z axis
// r: reset to original view
// p: pause
// q: quit
void solar_system::input(unsigned char key, int, int)
{
	switch (key)
	{
	case 'q': std::exit(EXIT_SUCCESS); break;
	case 'r': scene.tx = 0.0f; scene.ty = 0.0f; scene.tz = 0.0f; break;
	case 'p': scene.animate = !scene.animate; break;
	case 'a': scene.tx += 1.0f;  break;
	case 'z': scene.tx -= 1.0f;  break;
	case 's': scene.ty += 1.0f;  break;
	case 'x': scene.ty -= 1.0f;  break;
	case 'd': scene.tz += 1.0f;  break;
	case 'c': scene.tz -= 1.0f;  break;
	default: break;
	}
}
