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

#define SCREEN_WIDTH 256

#define ever ;;

/* 
 * 5 8bit texture pages are used here.
 * Both Polygons used for far-landscape and close-landscape use SCREEN_WIDTHxSCREEN_WIDTH
 * These pages are used for far landscae, near landescape and train window.
 * 2 pages (512xSCREEN_WIDTH) are used for each landscape. 1 page is used for
 * train window.
 * Each landscape is rap-rounded by 512 pixel.
 * Details of the sprite is as follows */
extern u_long far0[];		/* far mountains(left)*/
extern u_long far1[];		/* far mountains(right)*/
extern u_long near0[];		/* near mountains(left)*/
extern u_long near1[];		/* near mountains(right)*/
extern u_long window[];		/* train window*/

// Draws the combination of the 2 seamless textures to draw the background
static void draw_far_bg(Sprite_textured& far0_spr, Sprite_textured& far1_spr, int offset_x)
{
    // hold far0,far1 or far1,far0
    static Sprite_textured * sprites[2];

    // Figure out picture order (left side-right side or
    //                           right side-left side
    // dividing by 4 just prevents it from moving as fast
    //  (the background should scroll slower than the foreground)
    int left_right = (offset_x/4) & 0x100; // limit to 256 and wrap around
    int distance = (offset_x/4) & 0xFF;    // limit to 255 and wrap around

    sprites[0] = left_right ? &far1_spr : &far0_spr;
    sprites[1] = left_right ? &far0_spr : &far1_spr;
    
    // update UV coordinates
    sprites[0]->set_texture_offset(distance,0);
    sprites[1]->set_texture_offset(0,0);

    // update XY coordinates
    sprites[0]->set_pos(0,16);
    sprites[1]->set_pos(SCREEN_WIDTH-distance,16);

    // update width to draw
    sprites[0]->set_size(SCREEN_WIDTH-distance,168);
    sprites[1]->set_size(distance,168);

    // draw the far sprites
    sprites[0]->draw_ordered(3);
    sprites[1]->draw_ordered(3);
}

// Same as above function but for next layer (hills), middle level
static void draw_hills_bg(Sprite_textured& near0_spr, Sprite_textured& near1_spr, int offset_x)
{
    // hold far0,far1 or far1,far0
    static Sprite_textured * sprites[2];

    // Figure out picture order (left side-right side or
    //                           right side-left side
    // Here we divide by 2 instead of 4 so it 'moves' faster
    int left_right = (offset_x/2) & 0x100; // limit to 256
    int distance = (offset_x/2) & 0xFF;    // limit to 255

    sprites[0] = left_right ? &near1_spr : &near0_spr;
    sprites[1] = left_right ? &near0_spr : &near1_spr;
    
    // update UV coordinates
    sprites[0]->set_texture_offset(distance,0);
    sprites[1]->set_texture_offset(0,0);

    // update XY coordinates
    sprites[0]->set_pos(0,32);
    sprites[1]->set_pos(SCREEN_WIDTH-distance,32);

    // update width to draw
    sprites[0]->set_size(SCREEN_WIDTH-distance,168);
    sprites[1]->set_size(distance,168);

    // draw the far sprites
    // OT level 2 instead this time, so in front of the far bg
    sprites[0]->draw_ordered(2);
    sprites[1]->draw_ordered(2);
}

int main(void)
{
    Sprite_textured   far0_spr;
    Sprite_textured   far1_spr;
    Sprite_textured   near0_spr;
    Sprite_textured   near1_spr;
    Sprite_textured   window_spr;

    bool              draw_window = true;
    bool              draw_hills = true;
    bool              draw_city = true;

    Sprite_textured * left_right_spr[2]; // the left side and right side of the screen sprites 

    int               offset_x    = 0; // scrolling offset
    int               left_right  = 0; // which order to draw the left and right versions of each sprite
    int               velocity    = 1; // how fast to scroll
    
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
                          SCREEN_WIDTH, SCREEN_WIDTH,     // Texture size
                          0, 481        // CLUT VRAM X,Y
                        );
    far1_spr.load_texture(far1+0x80,    // texture data
                          far1,         // clut data
                          TEXTURE_8BIT, // BPP
                          512, SCREEN_WIDTH,     // VRAM Texture X,Y
                          SCREEN_WIDTH, SCREEN_WIDTH,     // Texture size
                          0, 482        // CLUT VRAM X,Y
                        );
    near0_spr.load_texture(near0+0x80,   // texture data
                           near0,        // clut data
                           TEXTURE_8BIT, // BPP
                           640, 0,       // VRAM Texture X,Y
                           SCREEN_WIDTH, SCREEN_WIDTH,     // Texture size
                           0, 483        // CLUT VRAM X,Y
                         );
    near1_spr.load_texture(near1+0x80,   // texture data
                           near1,        // clut data
                           TEXTURE_8BIT, // BPP
                           640, SCREEN_WIDTH,     // VRAM Texture X,Y
                           SCREEN_WIDTH, SCREEN_WIDTH,     // Texture size
                           0, 484        // CLUT VRAM X,Y
                         );
    window_spr.load_texture(window+0x80,   // texture data
                            window,        // clut data
                            TEXTURE_8BIT, // BPP
                            768, 0,     // VRAM Texture X,Y
                            SCREEN_WIDTH, SCREEN_WIDTH,     // Texture size
                            0, 485        // CLUT VRAM X,Y
                          );

    // window frame foreground
    window_spr.set_size(SCREEN_WIDTH,200);
    window_spr.set_pos(0,0);

    // hills mid background
    near0_spr.set_size(SCREEN_WIDTH, 168);
    near0_spr.set_pos(0,0);

    // hills - trees (connects to near0)
    near1_spr.set_size(SCREEN_WIDTH, 168);
    near1_spr.set_pos(0,0);

    // City/world background
    far0_spr.set_size(SCREEN_WIDTH, 168);
    far0_spr.set_pos(0, 0);

    far1_spr.set_size(SCREEN_WIDTH,168);
    far1_spr.set_pos(0,0);
    
    
    for (ever)
    {
        system->start_frame();

        // print some info
        FntPrint("Draw city, hills, window:\n %d,%d,%d\n", (int)draw_city,
                                                         (int)draw_hills,
                                                         (int)draw_window);
        FntPrint("Velocity: %d\n", velocity);

        // update offset position
        offset_x += velocity;

        // City and River background
        if (draw_city) {
            draw_far_bg(far0_spr, far1_spr, offset_x); // depth 3 - farthest
        }

        // Rolling hills mid ground
        if (draw_hills) {
            draw_hills_bg(near0_spr, near1_spr, offset_x); // depth 2 - mid
        }
        
        // Window foreground
        if (draw_window) {
            window_spr.draw_ordered(1); // depth 1 - nearest
        }


        // change some stuff based on controller
        pad1.read();
        if (pad1.is_held(PadLeft))  velocity--;
        if (pad1.is_held(PadRight)) velocity++;

        if (pad1.is_clicked(PadCross))    draw_window = !draw_window;
        if (pad1.is_clicked(PadCircle))   draw_hills = !draw_hills;
        if (pad1.is_clicked(PadTriangle)) draw_city = !draw_city;
        

        system->end_frame();
    }

    system->deinit();
    return 0;
}

