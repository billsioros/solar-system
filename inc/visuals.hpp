/*
Simplistic solar system visualization in OpenGL by

sdi1500144 - Vasileios Sioros
sdi1500176 - Ioannis Heilaris
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

    // A representation of a RGBA color
    struct Color
    {
        float red, green, blue, alpha;
        
        Color() : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f) {}

        Color(float red, float green, float blue, float alpha)
        :
        red(red), green(green), blue(blue), alpha(alpha)
        {}
    };

    // Created an abstraction layer for loading and rendering
    // a 3D model in wavefront format
    class Wavefront
    {
        struct Face { std::size_t fst[2], snd[2], thd[2]; };

        std::vector<detail::Vector3> vertices;
        std::vector<detail::Vector3> normals;
        std::vector<Face> faces;

    public:

        Wavefront(const char * wavefront_path);

        void render() const;
    };
}

namespace solar_system
{
    // A class describing the general characteristics
    // and funtionality of all the objects in our scene
    // That includes stars as well as planets
    class Star
    {
        friend class Planet;
        friend class Moon;
        friend class Earth;

    protected:

        // position: The position of the entity in the 3D space
        detail::Vector3 position;

        // size: Either the scale or the radius of the entity
        float size;

        // color: The color to be used when rendering the entity
        detail::Color color;

    public:

        Star(const detail::Vector3& position, float size, const detail::Color& color);

        virtual ~Star() {};

        virtual void render() const;

        virtual void update();
    };

    using Entity = Star;

    // A special case of a star, that also radiates
    class Sun : public Star
    {
        // ring_color: The original color of the sun's ring
        detail::Color ring_color;

        // dalpha: The rate of change of "ring_color"'s alpha
        float dalpha;

    public:

        Sun
        (
            const detail::Vector3& position,
            float size,
            const detail::Color& color,
            const detail::Color& ring_color,
            float dalpha
        );

        ~Sun() {};

        void render() const;

        void update();
    };

    // The planet abstract class inherits the characteristics and funcionalities
    // of the "Entity" class and defines some new general qualities
    // of planets
    class Planet : public Entity
    {
    protected:

        // rotating: Pointer to the entity, the entity at hand is rotating
        const Entity * rotating;

        // angle: The entity at hand is originally rotated
        // around the local y axis of the entity being pointed to
        // by "rotating" by "angle" degrees
        // dangle: The rate of change of "angle"
        // distance: The distance from the entity being pointed to by "rotating"
        float angle, dangle, distance;

    public:
        
        // wavefront: The 3D model used in rendering every single planet
        static const detail::Wavefront wavefront;

        Planet
        (
            const detail::Vector3& position,
            float size,
            const detail::Color& color,
            const Entity * rotating,
            float angle,
            float dangle,
            float distance
        );

        virtual ~Planet() {}

        virtual void render() const = 0;

        void update();
    };

    // A Planet specification for rendering the moon
    struct Moon : public Planet
    {
        Moon
        (
            const detail::Vector3& position,
            float size,
            const detail::Color& color,
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
            const detail::Vector3& position,
            float size,
            const detail::Color& color,
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
