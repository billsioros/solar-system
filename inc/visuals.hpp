
#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace utility
{
    struct Point { double x, y, z; };

    struct Color { std::uint8_t red, green, blue, alpha; };

    double rand(double min, double max);
    int rand(int min, int max);

    Point spherical2cartesian(double radius, double theta, double phi);
}

namespace solar_system
{
    class Planet
    {
        struct Face { std::size_t fst[2], snd[2], thd[2]; };

        utility::Point position;

        double scale;

        utility::Color color;

        std::vector<utility::Point> vertices;
        std::vector<utility::Point> normals;
        std::vector<Face> faces;

    public:
        
        Planet
        (
            const std::string& wavefront,
            const utility::Point& position,
            double scale,
            const utility::Color& color
        );

        void render() const;
    };

    struct Star
    {
        utility::Point position;

        std::size_t radius;

        utility::Color color;

    public:

        Star();
        Star(const utility::Point& position, std::size_t radius, const utility::Color& color);

        void render() const;
    };

    void setup();
    void render();
    void update();
}
