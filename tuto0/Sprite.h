// Sprite.h

#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

#include <stdio.h>

class Sprite
{
public:

    Sprite();

    // set the width and height of the sprite
    void set_size(const u_short width,
                  const u_short height);

protected:

    // with 4 points (a rectangle)
    POLY_F4 poly;

    // Position
    SVECTOR pos;

    // Size
    u_short width;
    u_short height;

};

class Sprite_textured : public Sprite
{
public:

    Sprite_textured();

    void set_size(const u_short width,
                  const u_short height);


private:
    
    // textured with 4 points (a rectangle)
    POLY_FT4 poly_tex;

    // Texture offset
    u_short u;
    u_short v;
};

#endif // end ifdef SPRITE_H_INCLUDED
