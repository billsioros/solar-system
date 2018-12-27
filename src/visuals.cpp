
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
#define STAR_COUNT (100UL)

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

// Return a random int in the range [min, max]
static int rand_range(int min, int max)
{
    return ((std::rand() % (max - min + 1)) + min);
}

// Return a point whose cartesian coordinates
// correspond to the spherical coordinates specified
static solar_system::detail::Vector3 spherical2cartesian
(
    double radius,
    double theta,
    double phi
)
{
    return
    {
        radius * std::sin(theta) * std::cos(phi),
        radius * std::sin(theta) * std::sin(phi),
        radius * std::cos(theta)
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
            std::cerr << "<WRN>: Ignoring line " + line << std::endl;
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
}

// Star class
// Save all the necessary info
// to render the star as a simple sphere
solar_system::Star::Star(const detail::Vector3& position, std::size_t radius, const detail::Color& color)
:
position(position), radius(radius), color(color)
{
}

void solar_system::Star::render() const
{
}

// Allocate the necessary resources
void solar_system::alloc()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    instance.sun = new Star
    (
        { 0.0, 0.0, 0.0 },
        100UL,
        { 255, 255, 0, 255 }
    );

    instance.ring = new Star
    (
        { 0.0, 0.0, 0.0 },
        150UL,
        { 255, 255, 0, 127 }
    );

    for (std::size_t s = 0UL; s < STAR_COUNT; s++)
    {
        const detail::Vector3 position =
        {
            rand_range(-100.0, 100.0),
            rand_range(-100.0, 100.0),
            rand_range(-100.0, 100.0)
        };

        const double radius = rand_range(10.0, 20.0);

        const detail::Color color =
        {
            255,
            255,
            0,
            static_cast<std::uint8_t>(rand_range(128, 255))
        };

        instance.stars.emplace_back(position, radius, color);
    }

    instance.earth = new Planet
    (
        __WAVEFRONT_OBJ__,
        spherical2cartesian(100.0, 45.0, 0.0),
        1.0,
        { 0, 255, 255, 255 }
    );

    instance.moon = new Planet
    (
        __WAVEFRONT_OBJ__,
        spherical2cartesian(300.0, 60.0, 00),
        0.5,
        { 51, 51, 51, 255 }
    );
}

void solar_system::setup()
{
    
}

void solar_system::render()
{
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
