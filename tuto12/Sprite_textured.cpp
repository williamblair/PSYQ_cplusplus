// Sprite_textured.cpp

#include "Sprite.h"

Sprite_textured::Sprite_textured()
{
    // initialize internal prim values
    SetSprt(&prims.sprite_prim);
    SetShadeTex(&prims.sprite_prim, 1); // disable shading

    // initialize texture offset
    u = v = 0;
    
    // Set size and offset within prim
    setXY0(&prims.sprite_prim, pos.vx, pos.vy);
    setUV0(&prims.sprite_prim, u, v);
    setWH(&prims.sprite_prim, width, height);
}

void Sprite_textured::set_size(const u_short width,
                               const u_short height)
{
    this->width  = width;
    this->height = height;

    setXY0(&prims.sprite_prim, pos.vx, pos.vy);
    setUV0(&prims.sprite_prim, u, v);
    setWH(&prims.sprite_prim, width, height);
}

void Sprite_textured::set_pos(const u_short x, const u_short y)
{
    pos.vx = x;
    pos.vy = y;

    setXY0(&prims.sprite_prim, pos.vx, pos.vy);
    setWH(&prims.sprite_prim, width, height);
}

void Sprite_textured::set_texture_offset(const u_short u, const u_short v)
{
    this->u = u;
    this->v = v;

    setUV0(&prims.sprite_prim, u, v);
    setWH(&prims.sprite_prim, width, height);
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
    setUV0(&prims.sprite_prim, 
            u,          // offset from top left of texture
            v);          
    setWH(&prims.sprite_prim,
          width,      // width and height from this offset to use
          height);    //   (assumes is the same as the sprite size)


//void SetDrawMode(
//    DR_MODE *p, // Pointer to drawing mode primitive
//    int dfe,    // 0: drawing not allowed in display area,
//            // 1: drawing allowed in display area
//
//    int dtd, // 0: dithering off, 1: dithering on.
//    int tpage, // Texture page 
//    RECT *tw) // Pointer to texture window 
//
//
//setDrawMode (p, dfe, dtd, tpage, tw) Macro version of SetDrawMode()
//
// Explanation
//
// Initializes a DR_MODE primitive. By using AddPrim() to insert a DR_MODE primitive into your primitive list, it
// is possible to change part of your drawing environment in the middle of drawing.
// If tw is 0, the texture window is not changed.

    SetDrawMode(&prims.dr_mode, 1, 1, texture.get_texture_page_id(), 0);
    prims.sprite_prim.clut = texture.get_clut_id();
    

    if (MargePrim(&prims.dr_mode, &prims.sprite_prim) != 0)
    {
        printf("Merge Failed!\n");
    }
}
    
void Sprite_textured::set_texture_vram_pos(const int x, const int y, TEXTURE_BPP bpp)
{
    texture.set_vram_pos(x, y, bpp);


    SetDrawMode(&prims.dr_mode, 1, 1, texture.get_texture_page_id(), 0);
    

    if (MargePrim(&prims.dr_mode, &prims.sprite_prim) != 0)
    {
        printf("Merge Failed!\n");
    }
}

void Sprite_textured::draw()
{
    DrawPrim(&prims.dr_mode);
}

void Sprite_textured::draw_ordered(int depth)
{
    static System * sys_ptr = System::get_instance();

    sys_ptr->add_prim( (void*)&prims.dr_mode, 
                        depth);
}


void Sprite_textured::copy_texture(const Sprite_textured& sprite)
{
    // Classes are automatically friends of themselves,
    // meaning class functions can auto access another instance's
    // private members
    texture.copy_from(sprite.texture);

    // Apply texture info to the primitive
    setUV0(&prims.sprite_prim,
            u,          // offset from top left of texture
            v);
    setWH(&prims.sprite_prim,
            width,      // width and height from this offset to use
            height);    //  (assumes is the same as the sprite size)

    SetDrawMode(&prims.dr_mode, 1, 1, texture.get_texture_page_id(), 0);
    prims.sprite_prim.clut  = texture.get_clut_id();
}

void Sprite_textured::move(const int x, const int y)
{
    pos.vx += x;
    pos.vy += y;

    setXY0(&prims.sprite_prim, pos.vx, pos.vy);
    setWH(&prims.sprite_prim, width, height);
}
