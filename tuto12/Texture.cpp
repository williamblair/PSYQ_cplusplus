// Texture.cpp

#include "Texture.h"

Texture::Texture()
{
    clut_id  = 0;
    tpage_id = 0;

    width  = 0;
    height = 0;

    was_loaded = false;
}

Texture::~Texture()
{
}

void Texture::load(u_long *texdata,  // pointer to texture data
                   u_long *clutdata, // pointer to CLUT data (NULL if none)
                   TEXTURE_BPP bpp,  // 0 - 16bit, 1 - 8bit, 2 - 4 bit
                   int x,        // where in VRAM to load the texture
                   int y,
                   int w,        // size of the texture
                   int h,
                   int clutX,    // where to upload the clut data (if given)
                   int clutY)
{
    if ( clutdata != NULL ) {
        clut_id = LoadClut(
            clutdata,         // clut pointer
            clutX, clutY      // VRAM coordinates
        );
    }

    tpage_id = LoadTPage(
        texdata,
        bpp,    // bit depth - 0=4bit, 1=8bit, 2=16bit
        0,      // semitransparency rate (default 0)
        x,      // x - destination from buffer address x
        y,      // y - destination frame buffer address y
        w,      // w - width
        h       // h - height
    );

    width  = w;
    height = h;

    was_loaded = true;
}

void Texture::copy_from(const Texture& texture)
{
    was_loaded = true;

    tpage_id = texture.tpage_id;
    clut_id  = texture.clut_id;
    width    = texture.width;
    height   = texture.height;
}

void Texture::apply_to_primitive(POLY_FT3 *prim, TEXCOORD *tex_coord)
{
    setUV3(
        prim,
        tex_coord->u0*width, // U0, V0
        tex_coord->v0*height,
        tex_coord->u1*width, // U1, V1
        tex_coord->v1*height,
        tex_coord->u2*width, // U2, V2
        tex_coord->v2*height
    );

    prim->tpage = tpage_id;
    prim->clut  = clut_id;
}


// Getters
u_short Texture::get_texture_page_id(void)
{
    return tpage_id;
}
u_short Texture::get_clut_id(void)
{
    return clut_id;
}
int Texture::get_width(void)
{
    return width;
}
int Texture::get_height(void)
{
    return height;
}


void Texture::set_vram_pos(const int x, const int y, TEXTURE_BPP bpp, int clut_x, int clut_y)
{
    // calculate the texture page itself (64x256 block)
//    int texture_x = x - (x % 64);
//    int texture_y = y - (y % 256);
    //int texture_x = x;
    //int texture_y = y;
    int texture_x = x;
    int texture_y = y;

    printf("Texture x,y: %d,%d\n", texture_x, texture_y);

    //if (texture_x % 64 != 0) {
    //    texture_x -= texture_x % 64;
   // }
    //if (texture_y % 256 != 0) {
    //    texture_y -= texture_y % 256;
    //}

    tpage_id = GetTPage((int)bpp,  // bits per pixel
                         0,      // semi-transparency rate
                         texture_x,      // vram x
                         texture_y);     // vram y

    clut_id = GetClut(clut_x, clut_y);
}

