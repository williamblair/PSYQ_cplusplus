// Sprite_textured.cpp

#include "Sprite.h"

Sprite_textured::Sprite_textured()
{
    // initialize internal prim values
    SetPolyFT4(&poly_tex);

    // initialize texture offset
    u = v = 0;
    
    setXYWH(&poly_tex, pos.vx, pos.vy, width, height);
    setUVWH(&poly_tex, u, v, width, height);
    setRGB0(&poly_tex, 128, 128, 128);
}

void Sprite_textured::set_size(const u_short width,
                               const u_short height)
{
    this->width  = width;
    this->height = height;

    setXYWH(&poly_tex, pos.vx, pos.vy, width, height);
    setUVWH(&poly_tex, u, v, width, height);
}

void Sprite_textured::load_texture(
        u_long *texdata,  // pointer to texture data
        u_long *clutdata, // pointer to CLUT data (NULL if none)
        TEXTURE_BPP bpp,  // 0 - 16bit, 1 - 8bit, 2 - 4 bit
        int x,        // where in VRAM to load the texture
        int y,
        int w,        // size of the texture
        int h,
        int clutX,    // where to upload the clut data (if given)
        int clutY)
{
    // load the memory
    texture.load(texdata,
                 clutdata,
                 bpp,
                 x,
                 y,
                 w,
                 h,
                 clutX,
                 clutY);


    // Apply texture info to the primitive
    setUVWH(&poly_tex, 
            u,          // offset from top left of texture
            v,          
            width,      // width and height from this offset to use
            height);    //   (assumes is the same as the sprite size)

    poly_tex.tpage = texture.get_texture_page_id();
    poly_tex.clut  = texture.get_clut_id();
    
}

void Sprite_textured::draw()
{
    DrawPrim(&poly_tex);
}

