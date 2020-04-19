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

// ball texture and clut data
extern u_long ball16x16[];
extern u_long ballclut[][8];

#define ever ;;

Sprite_textured ball;

// This sprite uses the displayed screen itself
// as its VRAM texture location
Sprite_textured screen_tex;

int main(void)
{
    System *          system      = System::get_instance();
    Pad               pad1;

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();
    system->init_3d();

    // the normal sprite to draw
    ball.load_texture(
        ball16x16,      // texture data
        ballclut[0],    // clut data
        TEXTURE_4BIT,   // BPP
        640, 0,         // VRAM tex x,y
        16, 16,         // Texture size
        0, 481          // VRAM clut x,y
    );
    ball.set_size(16, 16);
    ball.set_pos(10, 10);

    // the screen-sprite
    // will get its texture data from VRAM position 0,0
    // the VRAM is 16bit hence we use TEXTURE_16BIT
    screen_tex.set_texture_vram_pos(0, 0, TEXTURE_16BIT);
    screen_tex.set_size(20,20);
    screen_tex.set_pos(100, 100);

    for (ever)
    {
        system->start_frame();

        pad1.read();
        if (pad1.is_held(PadRight)) ball.move(5,0);
        if (pad1.is_held(PadLeft))  ball.move(-5,0);
        if (pad1.is_held(PadUp))    ball.move(0,-5);
        if (pad1.is_held(PadDown))  ball.move(0,5);

        ball.draw();
        screen_tex.draw();

        system->end_frame();
    }

    system->deinit();
    return 0;
}

