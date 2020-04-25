// Texture.h

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	// Not necessary but include it anyway
#include <string.h>
#include <stdlib.h>
#include <libetc.h>	// Includes some functions that controls the display
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <libgpu.h>	// GPU library header

// TODO - this is probably already defined somewhere in PSYQ
enum TEXTURE_BPP {
    TEXTURE_4BIT,   // 0
    TEXTURE_8BIT,   // 1
    TEXTURE_16BIT,  // 2
};

typedef struct {
    u_int u0;
    u_int v0;
    u_int u1;
    u_int v1;
    u_int u2;
    u_int v2;
} TEXCOORD;

class Texture
{
public:

    Texture();
    ~Texture();

    // Load texture data into VRAM
    void load(
        u_long *texdata,  // pointer to texture data
        u_long *clutdata, // pointer to CLUT data (NULL if none)
        TEXTURE_BPP bpp,  // 0 - 16bit, 1 - 8bit, 2 - 4 bit
        int x,        // where in VRAM to load the texture
        int y,
        int w,        // size of the texture
        int h,
        int clutX,    // where to upload the clut data (if given)
        int clutY
    );

    // Tell a primitive to use this texture
    void apply_to_primitive( POLY_FT3 *prim, TEXCOORD *tex_coord );

    // Use data from an existing texture
    void copy_from( const Texture& texture ); 

    // Getters
    u_short get_texture_page_id(void); // GetTPage() is already a function in PSYQ
    u_short get_clut_id        (void);

    int get_width (void);
    int get_height(void);

    // manually set where in VRAM the texture is located via updating the texture page id
    // note that the position will be aligned to the nearest previous valid texture page
    // location (each tpage 64pix width, 256pix height)
    // so whichever u,v coordinates used should be updated to compensate for this
    void set_vram_pos(const int x, const int y, TEXTURE_BPP bpp, int clut_x, int clut_y);

private:
    
    u_short tpage_id; // texture page number returned by LoadTPage
    u_short clut_id;  // CLUT number returned by LoadClut
    int width;
    int height;

    bool was_loaded; // true if load() has been called
};

#endif // TEXTURE_H_INCLUDED

