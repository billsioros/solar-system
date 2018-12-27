
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

// #include <GL/glut.h>

#define __WAVEFRONT_OBJ__ "/home/massiva/Documents/Courses/Graphics/data/planet.obj"

double utility::rand(double min, double max)
{
    return ((max - min) * (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX)) + min);
}

int utility::rand(int min, int max)
{
    return ((std::rand() % (max - min + 1)) + min);
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
            utility::Point point;
            
            ss >> point.x; ss >> point.y; ss >> point.z;

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

void solar_system::Planet::render() const
{
}

solar_system::Star::Star()
:
position({ utility::rand(-100.0, 100.0), utility::rand(-100.0, 100.0), utility::rand(-100.0, 100.0)}),
radius(utility::rand(10.0, 20.0)),
color({ 255, 255, 0, static_cast<std::uint8_t>(utility::rand(128, 255)) })
{
}

solar_system::Star::Star(const utility::Point& position, std::size_t radius, const utility::Color& color)
:
position(position), radius(radius), color(color)
{
}

void solar_system::Star::render() const
{
}

struct
{
    const std::size_t STAR_COUNT = 100UL;

    solar_system::Star * sun, * ring, * stars;

    solar_system::Planet * earth, * moon;
} instance;

void solar_system::setup()
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

    instance.stars = new Star[instance.STAR_COUNT];

    instance.earth = new Planet
    (
        __WAVEFRONT_OBJ__,
        utility::spherical2cartesian(100.0, 45.0, 0.0),
        1.0,
        { 0, 255, 255, 255 }
    );

    instance.moon = new Planet
    (
        __WAVEFRONT_OBJ__,
        utility::spherical2cartesian(300.0, 60.0, 00),
        0.5,
        { 51, 51, 51, 255 }
    );
}

void solar_system::render()
{
}

void solar_system::update()
{
    // code
}

void solar_system::dealloc()
{
    if (instance.sun)
        delete instance.sun;

    if (instance.ring)
        delete instance.ring;

    if (instance.stars)
        delete[] instance.stars;

    if (instance.earth)
        delete instance.earth;

    if (instance.moon)
        delete instance.moon;
}
