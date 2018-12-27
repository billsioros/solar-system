
#include <visuals.hpp>

#include <iostream>

#include <GL/glut.h>

void resize(int width, int height);

void inspect(unsigned char key, int mousex, int mousey);

int main(int argc, char * argv[])
{
    solar_system::setup();

    solar_system::dealloc();

    return 0;
}

void resize(int width, int height)
{
    if (!height)
        height = 1;

    if (!width)
        width = 1;
}

void inspect(unsigned char key, int mousex, int mousey)
{
    // code
}
