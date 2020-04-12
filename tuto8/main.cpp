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


#define ever ;;

/* 
 * 5 8bit texture pages are used here.
 * Both Polygons used for far-landscape and close-landscape use 256x256
 * These pages are used for far landscae, near landescape and train window.
 * 2 pages (512x256) are used for each landscape. 1 page is used for
 * train window.
 * Each landscape is rap-rounded by 512 pixel.
 * Details of the sprite is as follows */
extern u_long far0[];		/* far mountains(left)*/
extern u_long far1[];		/* far mountains(right)*/
extern u_long near0[];		/* near mountains(left)*/
extern u_long near1[];		/* near mountains(right)*/
extern u_long window[];		/* train window*/

int main(void)
{
    Sprite_textured   far0_spr;
    Sprite_textured   far1_spr;
    Sprite_textured   near0_spr;
    Sprite_textured   near1_spr;
    Sprite_textured   window_spr;
    
    System *          system      = System::get_instance();
    Pad               pad1;

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();

    // Load sprites
    far0_spr.load_texture(far0+0x80,    // texture data
                          far0,         // clut data
                          TEXTURE_8BIT, // BPP
                          512, 0,       // VRAM Texture X,Y
                          256, 256,     // Texture size
                          0, 481        // CLUT VRAM X,Y
                        );
    far1_spr.load_texture(far1+0x80,    // texture data
                          far1,         // clut data
                          TEXTURE_8BIT, // BPP
                          512, 256,     // VRAM Texture X,Y
                          256, 256,     // Texture size
                          0, 482        // CLUT VRAM X,Y
                        );
    near0_spr.load_texture(near0+0x80,   // texture data
                           near0,        // clut data
                           TEXTURE_8BIT, // BPP
                           640, 0,       // VRAM Texture X,Y
                           256, 256,     // Texture size
                           0, 483        // CLUT VRAM X,Y
                         );
    near1_spr.load_texture(near1+0x80,   // texture data
                           near1,        // clut data
                           TEXTURE_8BIT, // BPP
                           640, 256,     // VRAM Texture X,Y
                           256, 256,     // Texture size
                           0, 484        // CLUT VRAM X,Y
                         );
    window_spr.load_texture(window+0x80,   // texture data
                            window,        // clut data
                            TEXTURE_8BIT, // BPP
                            768, 0,     // VRAM Texture X,Y
                            256, 256,     // Texture size
                            0, 485        // CLUT VRAM X,Y
                          );
    
    for (ever)
    {
        system->start_frame();

        
        pad1.read();

        system->end_frame();
    }

    system->deinit();
    return 0;
}

