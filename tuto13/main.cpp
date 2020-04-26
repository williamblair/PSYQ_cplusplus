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

// the area for the balls to be drawn in, not the actual 
// screen size
#define SCREEN_WIDTH  100 
#define SCREEN_HEIGHT 50


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

#define MAX_BALLS 30 
Ball ball_sprites[MAX_BALLS];

class Viewport
{
public:

    Viewport()
    {
        r = rand() % 255;
        g = rand() % 255;
        b = rand() % 255;

        x = rand() % 250;
        y = rand() % 200;

        w = SCREEN_WIDTH  + (rand() % 40) - 20;
        h = SCREEN_HEIGHT + (rand() % 40) - 20;

        setRGB0(&drawenv, r,g,b);
        drawenv.isbg = 1;
    }

    void draw(DRAWENV* base_env, u_long* ot, const int otlen)
    {
        // are we currently drawing at 240 or 0 px vertically?
        int base_y = 0;
        
        base_y = base_env->clip.y;

        // Set our values
        SetDefDrawEnv(&drawenv, x, base_y + y, w, h);
        setRGB0(&drawenv, r,g,b);
        drawenv.isbg = 1;

        // Apply the drawing area
        PutDrawEnv(&drawenv);

        // Draw the OT
        DrawOTag(&ot[otlen-1]);
    }
    

private:
    DRAWENV drawenv;
    u_char r,g,b;

    int x,y;
    int w,h;
};

int main(void)
{
    int               i;
    int               num_balls = 1;
    System *          system      = System::get_instance();
    Pad               pad1;
    u_long *          cur_ot;
    
    #define MAX_VIEWPORTS 10
    DRAWENV           cur_draw_env;
    Viewport          viewports[MAX_VIEWPORTS];
    int               num_viewports = 5;

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();
    system->init_3d();
    
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

    for (ever)
    {
        system->start_frame();

        pad1.read();
        if (pad1.is_clicked(PadUp)) {
            ++num_balls;
            if (num_balls > MAX_BALLS) {
                num_balls = MAX_BALLS;
            }
        }
        if (pad1.is_clicked(PadDown)) {
            --num_balls;
            if (num_balls < 1) {
                num_balls = 1;
            }
        }
        if (pad1.is_clicked(PadRight)) {
            ++num_viewports;
            if (num_viewports > MAX_VIEWPORTS) {
                num_viewports = MAX_VIEWPORTS;
            }
        }
        if (pad1.is_clicked(PadLeft)) {
            --num_viewports;
            if (num_viewports < 1) {
                num_viewports = 1;
            }
        }


        for (i = 0; i < num_balls; ++i)
        {
            ball_sprites[i].move();
            ball_sprites[i].draw();
        }
        
        cur_ot = system->get_ot();

        // Skip drawing the usual order table
        //system->end_frame();

        // Instead, draw each of our custom viewports
        GetDrawEnv(&cur_draw_env); // save for after; like a stack push
        for (i = 0; i < num_viewports; ++i)
        {
            // OT_LEN defined in Display_buffer.h
            viewports[i].draw(&cur_draw_env, cur_ot, OT_LEN);
        }
        cur_draw_env.isbg = 0; // don't auto clear again
        PutDrawEnv(&cur_draw_env);
    
        FntPrint("Num Viewports: %d\n", num_viewports);
        FntPrint("Num Balls: %d\n", num_balls);

        FntFlush(-1);
        DrawSync(0);
        VSync(0);
    }

    system->deinit();
    return 0;
}

