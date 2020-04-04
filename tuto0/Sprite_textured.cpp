// Sprite_textured.cpp

#include "Sprite.h"

Sprite_textured::Sprite_textured()
{
    // initialize internal prim values
    SetPolyFT4(&poly_tex);

    // initialize texture offset
    u = v = 0;
}

void Sprite_textured::set_size(const u_short width,
                               const u_short height)
{
    this->width  = width;
    this->height = height;

    setXYWH(&poly_tex, pos.vx, pos.vy, width, height);
    setUVWH(&poly_tex, u, v, width, height);
}
