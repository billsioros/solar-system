
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
}

namespace solar_system
{
    class Body
    {
        friend class Planet;

    protected:

        detail::Vector3 position;

        float size;

        detail::Color color;

    public:

        Body(const detail::Vector3& position, float size, const detail::Color& color);

        virtual ~Body() {};

        virtual void render() const;

        virtual void update();
    };

    class Planet : public Body
    {
        struct Wavefront
        {
            struct Face { std::size_t fst[2], snd[2], thd[2]; };

            std::vector<detail::Vector3> vertices;
            std::vector<detail::Vector3> normals;
            std::vector<Face> faces;

            Wavefront(const char * wavefront_path);

            void render() const;

        } wavefront;
        
        const Body * rotating;

        float angle, velocity;

    public:
        
        Planet
        (
            const detail::Vector3& position,
            float size,
            const detail::Color& color,
            const char * wavefront_path,
            const Body * rotating,
            float angle,
            float velocity
        );

        ~Planet() {}

        void render() const;

        void update();
    };

    void alloc();
    void dealloc();

    void setup();
    void render();
    void update();
}
