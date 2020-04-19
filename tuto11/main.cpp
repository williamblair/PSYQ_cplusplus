// blank main template for psyq c++

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

#include "System.h"
#include "Sprite.h"
#include "Cube.h"
#include "Pad.h"


// number of cells/divisions to blur/'mosaic' the texture
#define NCELL_X 16
#define NCELL_Y 16

// Full texture size
#define TEX_WIDTH  256
#define TEX_HEIGHT 256

// Full drawing size (combined size of all prims to make mosaic)
#define DRAW_WIDTH 256
#define DRAW_HEIGHT 256

// polygon texture
extern u_long bgtex[];

// Texture loader
Texture prim_texture;

// primitive to draw the texture on
POLY_FT4 prims[NCELL_X*NCELL_Y];

static void init_prim()
{
    int i = 0;
    int row;
    int col;

    int cell_width = TEX_WIDTH / NCELL_X;
    int cell_height = TEX_HEIGHT / NCELL_Y;

    prim_texture.load(
        bgtex+0x80,     // texture data
        bgtex,          // clut data
        TEXTURE_4BIT,   // BPP
        640, 0,         // Texture x,y in VRAM
        TEX_WIDTH, TEX_HEIGHT,       // texture size
        0, 500          // clut x,y in VRAM
    );

    for (row = 0; row < NCELL_Y; ++row)
    {

        for (col = 0; col < NCELL_X; ++col)
        {

        SetPolyFT4(&prims[i]);
        SetShadeTex(&prims[i], 1);

        prims[i].tpage = prim_texture.get_texture_page_id();
        prims[i].clut = prim_texture.get_clut_id();

        setXYWH(&prims[i], col*cell_width, row*cell_height, cell_width, cell_height);
        setUVWH(&prims[i], col*cell_width, row*cell_height, cell_width-1, cell_height-1);

        i++;
        
        }
    }
}

#define ever ;;

int main(void)
{
    int i;
    int row, col;
    int sx, sy;
    int su, sv;
    int u, v;
    int x, y;
    int u0, v0;
    int u1, v1;
    int rate = 0; // mosaic division rate (0...7)
    System *          system      = System::get_instance();
    Pad               pad1;

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();
    system->init_3d();

    init_prim();

    for (ever)
    {
        system->start_frame();

        pad1.read();

        // user controls
        if (pad1.is_clicked(PadLeft)) rate--;
        if (pad1.is_clicked(PadRight)) rate++;

        if (rate > 7) rate = 7;
        if (rate < 0) rate = 0;

        // size of each primitive
        sx = TEX_WIDTH / NCELL_X;
        sy = TEX_HEIGHT / NCELL_X;

        // size of each UV texture area
        su = DRAW_WIDTH / NCELL_Y;
        sv = DRAW_HEIGHT / NCELL_Y;

        u = 0;
        v = 0;
        x = 0;
        y = 0;
        i = 0;

        for (row = 0; row < NCELL_Y; ++row)
        {
            // reset upper left texture X val
            u = 0;

            // reset primitive x val
            x = 0;
            
            for (col = 0; col < NCELL_X; ++col)
            {

                // calculate 'mosaic' look for u,v
                //  what you're doing is shrinking the texture area to use
                //  based on the rate variable, which is then stretched to the 
                //  size of the primitive, so it looks more blurry
                u0 = u + rate;
                v0 = v + rate;
                u1 = u + su - rate;
                v1 = v + sv - rate;

                // max size
                if (u1 > TEX_WIDTH-1) u1 = TEX_WIDTH-1;
                if (v1 > TEX_HEIGHT-1) v1 = TEX_HEIGHT-1;

                // apply and draw prim
                setUV4(&prims[i], u0, v0, // set UV coords in Z shape
                                  u1, v0, 
                                  u0, v1, 
                                  u1, v1); 
                setXYWH(&prims[i], x, y, sx, sy);
            
                system->add_prim(&prims[i], 1);
                i++;

                // increase upper left texture X val
                u += su;

                // increase primitive position
                x += sx;
            
            }

            // increase upper left texture Y val
            v += sv;

            // increase primitive y val
            y += sy;
        }

        system->end_frame();
    }

    system->deinit();
    return 0;
}

