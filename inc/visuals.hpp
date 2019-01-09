
#pragma once

#include <vector>

namespace detail
{
    struct Vector3
    {
        float x, y, z;

        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    };

    struct Color
    {
        float red, green, blue, alpha;
        
        Color() : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f) {}

        Color(float red, float green, float blue, float alpha)
        :
        red(red), green(green), blue(blue), alpha(alpha)
        {}
    };

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
    class Object
    {
        friend class Planet;
        friend class Moon;
        friend class Earth;

    protected:

        detail::Vector3 position;

        float size;

        detail::Color color;

    public:

        Object(const detail::Vector3& position, float size, const detail::Color& color);

        virtual ~Object() {};

        virtual void render() const;

        virtual void update();
    };

    using Star = Object;

    class Sun : public Star
    {
        detail::Color ring_color;

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

    class Planet : public Object
    {
    protected:

        const Object * rotating;

        float angle, dangle, distance;

    public:
        
        static const detail::Wavefront * wavefront;

        Planet
        (
            const detail::Vector3& position,
            float size,
            const detail::Color& color,
            const Object * rotating,
            float angle,
            float dangle,
            float distance
        );

        virtual ~Planet() {}

        virtual void render() const = 0;

        void update();
    };

    struct Moon : public Planet
    {
        Moon
        (
            const detail::Vector3& position,
            float size,
            const detail::Color& color,
            const Object * rotating,
            float angle,
            float dangle,
            float distance
        );

        void render() const;
    };

    struct Earth : public Planet
    {
        Earth
        (
            const detail::Vector3& position,
            float size,
            const detail::Color& color,
            const Object * rotating,
            float angle,
            float dangle,
            float distance
        );
        
        void render() const;
    };

    void alloc();
    void dealloc();

    void render();
    void update();

	void inspect(unsigned char key, int mousex, int mousey);
};
