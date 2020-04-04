// Sprite.cpp

#include "Sprite.h"

Sprite::Sprite()
{
    // initialize internal prim values
    SetPolyF4(&poly);
    SetShadeTex(&poly, 1); // ShadeText disable

    // initialize position
    pos.vx = pos.vy = pos.vz = 0;

    // Initialize color (brightness/tint if textured)
    setRGB0(&poly, 128, 128, 128);
}

void Sprite::set_size(const u_short width,
                      const u_short height)
{
    this->width  = width;
    this->height = height;

    setXYWH(&poly, pos.vx, pos.vy, width, height);
}

void Sprite::draw()
{
    DrawPrim(&poly);
}

