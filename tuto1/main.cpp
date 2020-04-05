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

int main(void)
{

    // First index: z index (order table)
    // 2nd   index: x index (left to right)
    // 3rd   index: y index (up to down)    
    Sprite_textured   balls[8][8][8];

    System *          system      = System::get_instance();
    u_int             z_index     = 0;
    u_int             y_index     = 0;
    u_int             x_index     = 0;

    system->init();
    system->init_graphics();

    for (z_index = 0; z_index < 8; ++z_index)
    {
        Sprite_textured * cur_ball = &balls[z_index][0][0];

        // load the initial texture
        // each z index uses a different CLUT
        // however the texture remains the same, so the texture data loading
        //  part is redundant - TODO
        cur_ball->load_texture(ball16x16,         // Texture data
                               ballclut[z_index], // CLUT data
                               TEXTURE_4BIT,      // BPP
                               640, 0,            // VRAM texture loc
                               16, 16,            // texture size
                               0, 481+z_index);   // CLUT VRAM loc

        cur_ball->set_pos(z_index * 2,
                          z_index * 2);
        cur_ball->set_size(16,16); // drawn texture size

        // copy the rest of the textures but save their positions
        for (x_index = 0; x_index < 8; ++x_index)
        {
            for (y_index = 0; y_index < 8; ++y_index)
            {
                cur_ball = &balls[z_index][x_index][y_index];

                cur_ball->copy_texture(balls[z_index][0][0]);
                cur_ball->set_pos(x_index * 10 + (z_index*2), 
                                 (y_index * 10) + (z_index * 2));
                cur_ball->set_size(16, 16);
            }
        }
    }

    for (ever)
    {
        system->start_frame();

        // draw the balls
        for (z_index = 0; z_index < 8; ++z_index)
        {
            for (x_index = 0; x_index < 8; ++x_index)
            {
                for (y_index = 0; y_index < 8; ++y_index)
                {
                    // draw using order table for z index ordering
                    // z index is the order table depth
                    balls[z_index][x_index][y_index].draw_ordered(z_index);

                    // Draw immediately (no ordering)
//                    balls[z_index][x_index][y_index].draw();
                }
            }
        }


        system->end_frame();
    }

    system->deinit();
    return 0;
}

