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

#include "Texture.h"

class Sprite
{
public:

    Sprite();

    // set the width and height of the sprite
    void set_size(const u_short width,
                  const u_short height);

    // immediately send the primitive to the GPU, not
    // in an order table
    void draw();

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

    // immediately send the primitive to the GPU, not
    // in an order table
    void draw();

    // Load texture contents into memory and apply to the primitive
    void load_texture(u_long *texdata,  // pointer to texture data
                      u_long *clutdata, // pointer to CLUT data (NULL if none)
                      TEXTURE_BPP bpp,  // 0 - 16bit, 1 - 8bit, 2 - 4 bit
                      int x,        // where in VRAM to load the texture
                      int y,
                      int w,        // size of the texture
                      int h,
                      int clutX,    // where to upload the clut data (if given)
                      int clutY);

private:
    
    // textured with 4 points (a rectangle)
    POLY_FT4 poly_tex;

    // Texture offset
    u_short u;
    u_short v;

    // Texture properties
    Texture texture;
};

#endif // end ifdef SPRITE_H_INCLUDED
