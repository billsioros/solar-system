
#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace solar_system
{
    namespace detail
    {
        struct Vector3 { double x, y, z; };

        struct Color { std::uint8_t red, green, blue, alpha; };
    }

    class Planet
    {
        struct Face { std::size_t fst[2], snd[2], thd[2]; };

        detail::Vector3 position;

        double scale;

        detail::Color color;

        std::vector<detail::Vector3> vertices;
        std::vector<detail::Vector3> normals;
        std::vector<Face> faces;

    public:
        
        Planet
        (
            const std::string& wavefront,
            const detail::Vector3& position,
            double scale,
            const detail::Color& color
        );

        void render() const;
    };

    struct Star
    {
        detail::Vector3 position;

        std::size_t radius;

        detail::Color color;

    public:

        Star(const detail::Vector3& position, std::size_t radius, const detail::Color& color);

        void render() const;
    };

    void alloc();

    void setup();
    void render();
    void update();

    void dealloc();
}
