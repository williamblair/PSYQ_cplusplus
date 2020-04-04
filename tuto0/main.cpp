// rewriting tutorial 1 in c++

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

#include "System.h"
#include "Sprite.h"

#define ever ;;

int main(void)
{
    Sprite_textured   sprite;
    System *          system      = System::get_instance();
    extern u_long     mat0[]; // sprite texture

    system->init();
    system->init_graphics();


    sprite.load_texture(mat0+0x80,    // texture data 
                        mat0,         // CLUT data
                        TEXTURE_4BIT, // Bits Per Pixel
                        640,          // VRAM texture position
                        0,
                        256,          // texture width
                        256,          // texture height
                        0,            // CLUT VRAM position
                        500);

    sprite.set_size(64, 64);

    for (ever)
    {
        system->start_frame();

        sprite.draw();

        system->end_frame();
    }

    system->deinit();
    return 0;
}


