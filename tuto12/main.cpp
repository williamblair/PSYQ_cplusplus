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


// max size of a sprite
#define SCREEN_WIDTH 255 
#define SCREEN_HEIGHT 240

#define ever ;;

// This sprite uses the area drawn offscreen (the balls)
// as its VRAM texture location
// each sprite has a height of 2; one sprite for every 2 lines on the screen
// this can actually be done with twice the amount with a height of 1 each
// but it wasn't working for me for some reason; haven't debugged it
static Sprite_textured screen_tex[SCREEN_HEIGHT/2];

static void curve_screen(const int amount)
{
    int main_offset = (320-SCREEN_WIDTH)/2;
    int i;
    for (i = 0; i < SCREEN_HEIGHT/2; ++i)
    {
        // sin wave curve
        int offset = rsin(i*ONE/(SCREEN_HEIGHT/2))*amount/ONE;
        //printf("Offset: %d\n", offset);
        screen_tex[i].set_pos(main_offset+offset, i*2);
        //screen_tex[i].set_size(SCREEN_WIDTH-offset, 2);
    }
}

// ball texture and clut data
extern u_long ball16x16[];
extern u_long ballclut[][8];

class Ball
{
public:

    void init_from_texture(
        u_long *texdata,  // pointer to texture data
        u_long *clutdata, // pointer to CLUT data (NULL if none)
        TEXTURE_BPP bpp,  // 0 - 16bit, 1 - 8bit, 2 - 4 bit
        int x,        // where in VRAM to load the texture
        int y,
        int w,        // size of the texture
        int h,
        int clutX,    // where to upload the clut data (if given)
        int clutY
    )
    {
        
        sprite.load_texture(
            ball16x16,      // texture data
            ballclut[0],    // clut data
            TEXTURE_4BIT,   // BPP
            640, 0,         // VRAM tex x,y
            16, 16,         // Texture size
            0, 481          // VRAM clut x,y
        );
        sprite.set_size(16, 16);
        x = 10; 
        y = 10;
        vel_x = rand()%5 + 1;
        vel_y = rand()%5 + 1;
        sprite.set_pos(10,10);
    }

    void init_from_ball(const Ball& base, int x, int y)
    {
        sprite.copy_texture(base.sprite);
        sprite.set_size(16,16);
        this->x = x;
        this->y = y;
        vel_x = rand()%5 + 1;
        vel_y = rand()%5 + 1;
        sprite.set_pos(x,y);
    }


    // auto move the ball around the coordinates given by SCREEN_WIDTH, SCREEN_HEIGHT
    void move()
    {
        x += vel_x;
        y += vel_y;
        
        if (x < 0) x = 0;
        if (x > SCREEN_WIDTH) x = SCREEN_WIDTH;
        
        if (y < 0) y = 0;
        if (y > SCREEN_HEIGHT) y = SCREEN_HEIGHT;
        
        if (x == SCREEN_WIDTH || x == 0) {
            bool vel_less_zero = (vel_x < 0);
            vel_x = rand()%10;
            if (!vel_less_zero) {
                vel_x *= -1;
            }
    
        }
        
        if (y == SCREEN_HEIGHT || y == 0) {
            bool vel_less_zero = (vel_y < 0);
            vel_y = rand()%10;
            if (!vel_less_zero) {
                vel_y *= -1;
            }
        }
        
        sprite.set_pos(x, y);
    }

    void draw()
    {
        sprite.draw_ordered(1);
    }

private:
    Sprite_textured sprite;
    int x;
    int y;
    int vel_x; // velocity
    int vel_y;
};


#define MAX_BALLS 100 
Ball ball_sprites[MAX_BALLS];

// The OT to draw the screen sprite with
#define SCR_SPR_OTLEN 8
u_long screen_spr_ot[2][SCR_SPR_OTLEN];
u_char cur_buf = 0;


int main(void)
{
    int i;
    int               num_balls = MAX_BALLS;
    System *          system      = System::get_instance();
    Pad               pad1;
    
    // Drawing environments (double buffered) for our screen sprite
    DRAWENV           screen_spr_drawenv[2];
    RECT              screen_rect;

    // amount to sin-wave the screen sprite
    int               curve_rate = 0;

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();
    system->init_3d();


    setRECT(&screen_rect, 0, 0, 320, 240);
    SetDefDrawEnv(&screen_spr_drawenv[0], 0, 0, 320, 240);
    SetDefDrawEnv(&screen_spr_drawenv[1], 0, 240, 320, 240);
    screen_spr_drawenv[0].isbg = screen_spr_drawenv[1].isbg = 1;
    setRGB0(&screen_spr_drawenv[0], 0,0,0); // rgb(0,0,0)
    setRGB0(&screen_spr_drawenv[1], 0,0,0); // rgb(0,0,0)

    //system->set_bg_color(0,100,100);

    // the normal sprites to draw
    ball_sprites[0].init_from_texture(
        ball16x16,      // texture data
        ballclut[0],    // clut data
        TEXTURE_4BIT,   // BPP
        640, 0,         // VRAM tex x,y
        16, 16,         // Texture size
        0, 481          // VRAM clut x,y
    );
    for (i = 1; i < MAX_BALLS; ++i)
    {
        ball_sprites[i].init_from_ball(ball_sprites[0], 
                                       10 * (i+1),      // x
                                       10 * (i+1));     // y
    }

    for (i = 0; i < SCREEN_HEIGHT/2; ++i)
    {
        // the screen-sprite
        // will get its texture data from VRAM position 0,0
        // the VRAM is 16bit hence we use TEXTURE_16BIT
        screen_tex[i].set_vram_pos(320, i*2, TEXTURE_16BIT, 0, 481);
        screen_tex[i].set_size(SCREEN_WIDTH,2);
        screen_tex[i].set_pos((320-SCREEN_WIDTH)/2, i*2);
    }

    for (ever)
    {
        pad1.read();
        
        VSync(0);
        DrawSync(0);
        
        system->start_frame();


        // offset each of the screen texture lines
        if (pad1.is_held(PadL1)) {
            curve_rate++;
            curve_screen(curve_rate);
        }
        if (pad1.is_held(PadR1)) {
            curve_rate--;
            curve_screen(curve_rate);
        }

        for (i = 0; i < num_balls; ++i)
        {
            ball_sprites[i].move();
            ball_sprites[i].draw();
        }

        // Send the regular, normal OT to the GPU and wait for it to finish
        system->end_frame();

        // Set our screen sprite drawing area
        // notice we set !cur_buf instead of cur_buf
        PutDrawEnv(&screen_spr_drawenv[!cur_buf]);

        // Draw our custom screen sprite order table
        ClearOTagR((u_long*)screen_spr_ot[cur_buf], SCR_SPR_OTLEN);
        // Draw each screen line
        for (i = 0; i < SCREEN_HEIGHT/2; ++i)
        {
            screen_tex[i].add_to_ot((u_long*)screen_spr_ot[cur_buf], 2);
        }
        // Send all the lines to the gpu
        DrawOTag(&screen_spr_ot[cur_buf][SCR_SPR_OTLEN-1]);
        
        cur_buf = !cur_buf;
        FntFlush(-1);

    }

    system->deinit();
    return 0;
}

