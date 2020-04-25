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

// max size of a sprite
#define SCREEN_WIDTH 255 
#define SCREEN_HEIGHT 240

#define ever ;;

// The OT to draw the screen sprite with
#define SCR_SPR_OTLEN 8 
u_long screen_spr_ot[2][SCR_SPR_OTLEN];
u_char cur_buf = 0;

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

typedef struct Ball
{
    Sprite_textured sprite;
    int x;
    int y;
    int vel_x; // velocity
    int vel_y;
} Ball;

#define NUM_BALLS 100 
Ball ball_sprites[NUM_BALLS];

static void move_ball(Ball* ball)
{
    ball->x += ball->vel_x;
    ball->y += ball->vel_y;
    if (ball->x < 0) ball->x = 0;
    if (ball->x > SCREEN_WIDTH) ball->x = SCREEN_WIDTH;
    if (ball->y < 0) ball->y = 0;
    if (ball->y > SCREEN_HEIGHT) ball->y = SCREEN_HEIGHT;
    if (ball->x == SCREEN_WIDTH || ball->x == 0) {
        bool vel_less_zero = (ball->vel_x < 0);
        ball->vel_x = rand()%10;
        if (!vel_less_zero) {
            ball->vel_x *= -1;
        }

    }
    if (ball->y == SCREEN_HEIGHT || ball->y == 0) {
        bool vel_less_zero = (ball->vel_y < 0);
        ball->vel_y = rand()%10;
        if (!vel_less_zero) {
            ball->vel_y *= -1;
        }
    }
    ball->sprite.set_pos(ball->x, ball->y);
}

int main(void)
{
    int i;
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
    ball_sprites[0].sprite.load_texture(
        ball16x16,      // texture data
        ballclut[0],    // clut data
        TEXTURE_4BIT,   // BPP
        640, 0,         // VRAM tex x,y
        16, 16,         // Texture size
        0, 481          // VRAM clut x,y
    );
    ball_sprites[0].sprite.set_size(16, 16);
    ball_sprites[0].x = 10; 
    ball_sprites[0].y = 10;
    ball_sprites[0].vel_x = rand()%10;
    ball_sprites[0].vel_y = rand()%10;
    ball_sprites[0].sprite.set_pos(10,10);

    for (i=1; i<NUM_BALLS; ++i)
    {
        ball_sprites[i].sprite.copy_texture(ball_sprites[0].sprite);
        ball_sprites[i].sprite.set_size(16,16);
        ball_sprites[i].x = 10*(i+1); 
        ball_sprites[i].y = 10*(i+1);
        ball_sprites[i].vel_x = rand()%10;
        ball_sprites[i].vel_y = rand()%10;
        ball_sprites[i].sprite.set_pos(ball_sprites[i].x,ball_sprites[i].y);
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

        for (i = 0; i < NUM_BALLS; ++i)
        {
            move_ball(&ball_sprites[i]);
            ball_sprites[i].sprite.draw_ordered(1);
            //ball_sprites[i].sprite.draw();
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

