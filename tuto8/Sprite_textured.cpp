// Sprite_textured.cpp

#include "Sprite.h"

Sprite_textured::Sprite_textured()
{
    // initialize internal prim values
    //SetPolyFT4(&poly_tex);

    SetSprt(&sprite_prim);
    SetShadeTex(&sprite_prim, 1); // disable shading

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

void Sprite_textured::set_pos(const u_short x, const u_short y)
{
    pos.vx = x;
    pos.vy = y;

    setXYWH(&poly_tex, pos.vx, pos.vy, width, height);
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

    //poly_tex.tpage = texture.get_texture_page_id();
    //poly_tex.clut  = texture.get_clut_id();

    SetDrawMode(&dr_mode, 1, 1, texture.get_texture_page_id(), 0);
    
}

void Sprite_textured::draw()
{
    DrawPrim(&poly_tex);
}

void Sprite_textured::draw_ordered(int depth)
{
    static System * sys_ptr = System::get_instance();

    sys_ptr->add_prim( (void*)&poly_tex, 
                        depth);
}


void Sprite_textured::copy_texture(const Sprite_textured& sprite)
{
    // Classes are automatically friends of themselves,
    // meaning class functions can auto access another instance's
    // private members
    texture.copy_from(sprite.texture);

    // Apply texture info to the primitive
    setUVWH(&poly_tex, 
            u,          // offset from top left of texture
            v,          
            width,      // width and height from this offset to use
            height);    //   (assumes is the same as the sprite size)

    poly_tex.tpage = texture.get_texture_page_id();
    poly_tex.clut  = texture.get_clut_id();
}

