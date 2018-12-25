
#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace utility
{
    struct Point { double x, y, z; };

    struct Color { std::uint8_t red, green, blue, alpha; };
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

        void render();
    };

    struct Star
    {
        utility::Point position;

        std::size_t radius;

        utility::Color color;

    public:

        Star(const utility::Point& position, std::size_t radius, const utility::Color& color);

        void render();
    };

    void setup();
    void render();
    void update();

    void keyboard(unsigned char key, int mousex, int mousey);
}
