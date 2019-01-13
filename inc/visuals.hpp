/*
Simplistic solar system implemented in OpenGL

* 1115201500144 - Vasileios Sioros
* 1115201500176 - Ioannis Heilaris
*/

#pragma once

#include <vector>

namespace detail
{
    // A representation of a 3D vector
    struct Vector3
    {
        float x, y, z;

        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    };

    // A representation of an RGBA color
    struct Color
    {
        float red, green, blue, alpha;
        
        Color() : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f) {}

        Color(float red, float green, float blue, float alpha)
        :
        red(red), green(green), blue(blue), alpha(alpha)
        {}
    };

    // An abstraction layer for loading and rendering
    // a 3D model in wavefront format
    class Wavefront
    {
        struct Face { std::size_t fst[2], snd[2], thd[2]; };

        std::vector<detail::Vector3> vertices;
        std::vector<detail::Vector3> normals;
        std::vector<Face> faces;

    public:

        Wavefront(const char * wavefront_path);

        void render(float scale) const;
    };
}

namespace solar_system
{
    // A class describing the general characteristics
    // and funtionalities of all objects in our scene
    struct Entity
    {
        Entity() = default;
        ~Entity() = default;

        virtual void render() const = 0;
        virtual void update() {}
    };

    struct Star : public Entity, public detail::Vector3
    {
        Star(const detail::Vector3& position) : detail::Vector3(position) {}
        ~Star() = default;

        void render() const;
    };

    class Sun : public Entity
    {
        // @ring_color: The original color of the sun's ring
        detail::Color ring_color;

    public:

        Sun(const detail::Color& ring_color);
        ~Sun() = default;

        void render() const;

        void update();
    };

    // The planet abstract class inherits the characteristics and funcionalities
    // of the "Entity" class and defines some new general qualities of planets
    class Planet : public Entity
    {
    protected:

        // @rotating: Pointer to the entity, the planet is rotating
        const Entity * rotating;

        // @angle: A float that oscillates in the range [0.0f, 360.0f] and
        //  is a measurement of the amount that the planet is rotated
        // about @rotating's center
        // @dangle: @angle's rate of change
        // @distance: The distance between @rotating's and the planet's centers
        float angle, dangle, distance;

    public:
        
        // @wavefront: The 3D model used to render every single planet
        static const detail::Wavefront wavefront;

        Planet
        (
            const Entity * rotating,
            float angle,
            float dangle,
            float distance
        );

        virtual void render() const = 0;

        void update();
    };

    // A Planet specification for rendering the moon
    struct Moon : public Planet
    {
        Moon
        (
            const Entity * rotating,
            float angle,
            float dangle,
            float distance
        );

        void render() const;
    };

    // A Planet specification for rendering earth
    struct Earth : public Planet
    {
        Earth
        (
            const Entity * rotating,
            float angle,
            float dangle,
            float distance
        );
        
        void render() const;
    };

    void setup();
    void render();
    void update();

	void input(unsigned char key, int mousex, int mousey);
};
