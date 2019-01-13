/*
Simplistic solar system implemented in OpenGL

* 1115201500144 - Vasileios Sioros
* 1115201500176 - Ioannis Heilaris
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

// ATTENTION:
// CHANGE THE FOLLOWING MACRO IN CASE YOU HAVE A DIFFERENT FILE SYSTEM TREE STRUCTURE
#if !defined(__WAVEFRONT_PATH__)
    #define __WAVEFRONT_PATH__ "/home/massiva/Documents/Courses/Graphics/data/planet.obj"
    // #define __WAVEFRONT_PATH__ "C:/Users/johnf/Source/Repos/solar-system/data/planet.obj"
#endif

// A macro function returning
// a random floating point number in the range [min, max]
#define random(min, max)                                                \
(                                                                       \
    (max - min) *                                                       \
    (                                                                   \
        static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)  \
    ) + min                                                             \
)                                                                       \

// Sun info:
#define SUN_POSITION        0.0f, 0.0f, -100.0f
#define SUN_SIZE            (20.0f)
#define SUN_SLICES          (100)
#define SUN_COLOR           1.0f, 0.647f, 0.078f, 1.0f

// Star info:
#define STAR_COUNT          (300UL)

#define STAR_POSITION       spherical(random(300.0f, 450.0f), random(0.0f, 180.0f), random(0.0f, 180.0f), { SUN_POSITION })
#define STAR_SIZE           (0.5f)
#define STAR_SLICES         (10)
#define STAR_COLOR          1.0f, 0.964f, 0.756f, 1.0f

// Ring info:
#define RING_SIZE           (SUN_SIZE * (12.0f / 11.0f))
#define RING_DALPHA         (0.0025f)
#define RING_ALPHA_MIN      (0.05f)
#define RING_ALPHA_MAX      (0.7f)
#define RING_COLOR          { 1.0f, 0.89f, 0.078f, RING_ALPHA_MIN }

// Earth Info:
#define EARTH_SCALE         (0.00625f)
#define EARTH_COLOR         0.0f, 0.435f, 0.639f, 1.0f
#define EARTH_ANGLE         (-90.0f)
#define EARTH_DANGLE        (0.0625f)
#define EARTH_DISTANCE      (60.0f)

// Moon Info:
#define MOON_SCALE          (EARTH_SCALE / 4.0f)
#define MOON_COLOR          0.219f, 0.219f, 0.219f, 1.0f
#define MOON_ANGLE          (30.0f)
#define MOON_DANGLE         (1.0f)
#define MOON_DISTANCE       (EARTH_DISTANCE / 6.0f)

// An unnamed struct that holds all the info having to do with our scene
static struct
{
    // @paused: a bool indicating if the user has pressed the pause button
    // @crx, @cry, @crz: the camera's angle of rotation around the sun
    // in the corresponding axis
    // @entities: A vector that contains every entity to be rendered
    bool paused;
    float crx, cry, crz;
    std::vector<std::unique_ptr<solar_system::Entity>> entities;
} scene =
{
    .paused = false,
    .crx = 0.0f, .cry = 0.0f, .crz = 0.0f,
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

// Render the wavefront 3D entity
// Simply scale the resulting entity by the amount specified
// to get the desirable result
void detail::Wavefront::render(float scale) const
{
    glPushMatrix();

    glScalef(scale, scale, scale);

    glBegin(GL_TRIANGLES);

    for (const auto& face : faces)
    {
        const auto& v1 = vertices[face.fst[0UL] - 1UL];
        const auto& v2 = vertices[face.snd[0UL] - 1UL];
        const auto& v3 = vertices[face.thd[0UL] - 1UL];

        const auto& n1 = normals[face.fst[1UL] - 1UL];
        const auto& n2 = normals[face.snd[1UL] - 1UL];
        const auto& n3 = normals[face.thd[1UL] - 1UL];

        glNormal3f(n1.x, n1.y, n1.z);
        glVertex3f(v1.x, v1.y, v1.z);

        glNormal3f(n2.x, n2.y, n2.z);
        glVertex3f(v2.x, v2.y, v2.z);

        glNormal3f(n3.x, n3.y, n3.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }

    glEnd();

    glPopMatrix();
}

// Star class
// In order to render a star we translate to its position
// and draw a colored solid sphere
void solar_system::Star::render() const
{
    glColor4f(STAR_COLOR);

    glPushMatrix();

    glTranslatef(x, y, z);

    glutSolidSphere(STAR_SIZE, STAR_SLICES, STAR_SLICES);

    glPopMatrix();
}

// Sun class
solar_system::Sun::Sun(const detail::Color& ring_color)
:
ring_color(ring_color)
{
}

// In order to render the sun, we render 2 concentric solid spheres.
// So we need to translate to the sun's position and
// draw 2 solid spheres of different sizes and colors
void solar_system::Sun::render() const
{
    glPushMatrix();

    glTranslatef(SUN_POSITION);

    glColor4f(SUN_COLOR);

    glutSolidSphere(SUN_SIZE, SUN_SLICES, SUN_SLICES);

    glColor4f(ring_color.red, ring_color.green, ring_color.blue, ring_color.alpha);

    glutSolidSphere(RING_SIZE, 50, 50);

    glPopMatrix();
}

// Update the sun's ring's color, so that its alpha oscillates between
// 2 values, with the purpose of making it seem like it's radiating
void solar_system::Sun::update()
{
    static float dalpha = RING_DALPHA;

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
    const Entity * rotating,
    float angle,
    float dangle,
    float distance
)
:
rotating(rotating),
angle(angle),
dangle(dangle),
distance(distance)
{
}

// Update the planet's angle of rotation so that it oscillates between 0 and 360 degrees
void solar_system::Planet::update()
{
    angle += (angle < 360.0f ? dangle : -360.0f);
}

// Earth class
solar_system::Earth::Earth
(
    const Entity * rotating,
    float angle,
    float dangle,
    float distance
)
:
Planet(rotating, angle, dangle, distance)
{
}

// Translate to the sun's location, rotate by angle in the y axis
// and translate EARTH_DISTANCE away in the x axis
// Finally render the wavefront model scaled by EARTH_SCALE
// This results in the earth orbiting the sun in its local y axis
// ATTENTION: The matching "glPopMatrix" to the following "glPushMatrix"
// can be found in solar_system::Moon::render
void solar_system::Earth::render() const
{
    glColor4f(EARTH_COLOR);

    glPushMatrix();

    glTranslatef(SUN_POSITION);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(EARTH_DISTANCE, 0.0f, 0.0f);

    Planet::wavefront.render(EARTH_SCALE);
}

// Moon class
solar_system::Moon::Moon
(
    const Entity * rotating,
    float angle,
    float dangle,
    float distance
)
:
Planet(rotating, angle, dangle, distance)
{
}

// After rendering the earth, our 3D system has been translated
// and rotated as need. We now rotate by angle in the earth's local x axis
// (due to the previously referenced transformations) and then
// translate MOON_DISTANCE away in the y-axis
// Finally render the wavefront model scaled by MOON_SCALE
// This results in the moon rotating the earth in each local x axis
void solar_system::Moon::render() const
{
    glColor4f(MOON_COLOR);

    glRotatef(angle, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, MOON_DISTANCE, 0.0f);

    Planet::wavefront.render(MOON_SCALE);

    glPopMatrix();
}

// Seed the random number generator and
// initialize all the entities that make up our scene
// and pass them to a vector
void solar_system::setup()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (std::size_t s = 0UL; s < STAR_COUNT; s++)
        scene.entities.emplace_back(new Star(STAR_POSITION));

    Sun * sun = new Sun(RING_COLOR);

    Earth * earth = new Earth
    (
        sun,
        EARTH_ANGLE,
        EARTH_DANGLE,
        EARTH_DISTANCE
    );

    Moon * moon = new Moon
    (
        earth,
        MOON_ANGLE,
        MOON_DANGLE,
        MOON_DISTANCE
    );

    scene.entities.emplace_back(earth);

    scene.entities.emplace_back(moon);

    scene.entities.emplace_back(sun);
}

// Clear the color and depth buffers and load the identity matrix
// Translate to the sun's position (i.e. the center of our world)
// and rotate the camera according to the user's input
// The entities are rendered in the order in which
// they were introduced into the vector
// After rendering every entity, draw the 3D space axes
// and swap buffers
void solar_system::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -100.f);
	glRotatef(scene.crx, 1.0f, 0.0f, 0.0f);
	glRotatef(scene.cry, 0.0f, 1.0f, 0.0f);
	glRotatef(scene.crz, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, 100.f);

    for (const auto& entity_ptr : scene.entities)
    {
        entity_ptr->render();
    }

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

// If the user hasn't paused the program,
// update each entity and mark the scene as needing to be redisplayed
void solar_system::update()
{
	if(!scene.paused)
    {
		for (auto& entity_ptr : scene.entities)
			entity_ptr->update();
    }

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
	case 'r': scene.crx = 0.0f; scene.cry = 0.0f; scene.crz = 0.0f; break;
	case 'p': scene.paused = !scene.paused; break;
	case 'a': scene.crx += 1.0f;  break;
	case 'z': scene.crx -= 1.0f;  break;
	case 's': scene.cry += 1.0f;  break;
	case 'x': scene.cry -= 1.0f;  break;
	case 'd': scene.crz += 1.0f;  break;
	case 'c': scene.crz -= 1.0f;  break;
	default: break;
	}
}
