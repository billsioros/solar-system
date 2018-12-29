
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

// File system tree specific Wavefront .obj path
#define __WAVEFRONT_OBJ__ "/home/massiva/Documents/Courses/Graphics/data/planet.obj"

// Number of stars to be created
#define STAR_COUNT (50UL)

// A struct containing all the information about our simulation
static struct
{
    solar_system::Star * sun, * ring;
    
    std::vector<solar_system::Star> stars;

    solar_system::Planet * earth, * moon;
} instance;

// Return a random double between 0 and 1
static double rand_range()
{
    return static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX);
}

// Return a random double in the range [min, max]
static double rand_range(double min, double max)
{
    return ((max - min) * rand_range() + min);
}

// Return a point whose cartesian coordinates
// correspond to the spherical coordinates specified
static solar_system::detail::Vector3 spherical2cartesian
(
    double radius,
    double theta,
    double phi,
    const solar_system::detail::Vector3& center = { 0.0, 0.0, 0.0 }
)
{
    return
    {
        radius * std::sin(theta) * std::cos(phi) + center.x,
        radius * std::sin(theta) * std::sin(phi) + center.y,
        radius * std::cos(theta) + center.z
    };
}

// Planet class
// Parse the specified wavefront file
// and save all the information necessary
// to render it as a 3d object
solar_system::Planet::Planet
(
    const std::string& wavefront,
    const detail::Vector3& position,
    double scale,
    const detail::Color& color
)
: position(position), scale(scale), color(color)
{
    std::ifstream ifs(wavefront);

    if (!ifs.is_open())
    {
        std::perror(("<ERR>: Failed to open file " + wavefront).c_str());
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
        std::perror(("<ERR>: Failed to parse file " + wavefront).c_str());
        std::exit(EXIT_FAILURE);
    }
}

void solar_system::Planet::render() const
{
    glColor4f(color.red, color.green, color.blue, color.alpha);

    glPushMatrix();
    
    glTranslated(position.x, position.y, position.z);

    glScaled(scale, scale, scale);

    glBegin(GL_TRIANGLES);

    for (const auto& face : faces)
    {
        const auto& v1 = vertices[face.fst[0] - 1];
        const auto& v2 = vertices[face.snd[0] - 1];
        const auto& v3 = vertices[face.thd[0] - 1];

        glVertex3d(v1.x, v1.y, v1.z);
        glVertex3d(v2.x, v2.y, v2.z);
        glVertex3d(v3.x, v3.y, v3.z);
    }

    glEnd();

    glPopMatrix();
}

// Star class
// Save all the necessary info
// to render the star as a simple sphere
solar_system::Star::Star(const detail::Vector3& position, double radius, const detail::Color& color)
:
position(position), radius(radius), color(color)
{
}

void solar_system::Star::render() const
{
    glColor4f(color.red, color.green, color.blue, color.alpha);

    glPushMatrix();

    glLoadIdentity();

    glTranslated(position.x, position.y, position.z);

    glutSolidSphere(radius, 50, 50);

    glPopMatrix();
}

// Allocate the necessary resources
void solar_system::alloc()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const detail::Vector3 sun_position = { 0.0, 0.0, -75.0 };

    const double sun_radius = 10.0;

    detail::Vector3 position = sun_position;

    double radius = sun_radius;

    detail::Color color = { 0.9, 0.5, 0.0, 1.0 };

    instance.sun = new Star(position, radius, color);

    radius *= 5.0 / 4.0;

    color = { 1.0, 1.0, 1.0, color.alpha / 4 };

    instance.ring = new Star(position, radius, color);

    for (std::size_t s = 0UL; s < STAR_COUNT; s++)
    {
        position =
        {
            rand_range(-180.0, 180.0),
            rand_range(-90.0, 90.0),
            rand_range(-200.0, -300.0)
        };

        radius = rand_range(1.5, 2.0);

       color = { 1.0, 0.9, 0.6, 1.0 };

        instance.stars.emplace_back(position, radius, color);
    }

    position = spherical2cartesian(4 * sun_radius, 90.0, 0.0, sun_position);

    color = { 0.0, 1.0, 1.0, 1.0 };

    instance.earth = new Planet(__WAVEFRONT_OBJ__, position, 0.00625, color);

    position = spherical2cartesian(4 * sun_radius, 90.0, 0.0, position);

    color = { 0.3, 0.3, 0.3, 1.0 };

    instance.moon = new Planet(__WAVEFRONT_OBJ__, position, 0.003125, color);
}

void solar_system::setup()
{
    
}

void solar_system::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    for (const auto& star : instance.stars)
        star.render();

    instance.ring->render();

    instance.sun->render();

    instance.earth->render();

    instance.moon->render();

    glutSwapBuffers();
}

void solar_system::update()
{
    // code
}

// Deallocate all the resources
void solar_system::dealloc()
{
    if (instance.sun)
        delete instance.sun;

    if (instance.ring)
        delete instance.ring;

    if (instance.earth)
        delete instance.earth;

    if (instance.moon)
        delete instance.moon;
}
