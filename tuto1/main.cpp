// Tutorial 1

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

#include "System.h"
#include "Sprite.h"

#define ever ;;

// 16x16 ball texture pattern
extern u_long ball16x16[];

// ball CLUT (16 colors x 32)
extern u_long ballclut[][8];

void init_prim()
{
}

int main(void)
{
    
    Sprite_textured   balls[2];
    System *          system      = System::get_instance();

    system->init();
    system->init_graphics();

    // load the initial texture
    balls[0].load_texture(ball16x16,        // Texture data
                          ballclut[0],      // CLUT data
                          TEXTURE_4BIT,     // BPP
                          640, 0,           // VRAM texture loc
                          16, 16,           // texture size
                          0, 481);          // CLUT VRAM loc

    balls[0].set_pos(150,100);
    balls[0].set_size(16,16); // drawn texture size

    // copy the texture into the next sprite
    balls[1].copy_texture(balls[0]);
    balls[1].set_size(16,16);
    balls[1].set_pos(100,100);

    for (ever)
    {
        system->start_frame();

        balls[0].draw();
        balls[1].draw();


        system->end_frame();
    }

    system->deinit();
    return 0;
}

