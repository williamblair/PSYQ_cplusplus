// System.h

#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

#include <stdio.h>

#include "Display_buffer.h"

// 4 split screen in this example
#define NUM_VIEWPORTS 4
#define VIEWPORT_WIDTH  (320/2)
#define VIEWPORT_HEIGHT (240/2)

// Singleton system class
class System
{
public:

    static System * get_instance()
    {
        return &instance_obj;
    }

    // initialize playstation internals
    // not ready to draw after this -
    // graphics initialization still required
    void init();

    // Set up drawing environments
    void init_graphics();

    // Set up 3d drawing properties
    void init_3d();

    // Set the display area in VRAM to send to the TV
    void set_dispenv();

    // Set up drawing for the current frame
    void start_frame(const int viewport_index);

    // Send the current frame to be drawn
    void end_frame(const int viewport_index);

    // stop the system; a new program
    // can be called with Exec() at this point,
    // so that's pretty cool
    void deinit();

    // Add a primitive to the order table at the given depth
    void add_prim(void * prim, int depth, const int viewport_index);

private:

    // private singleton constructor
    System() :
        cur_buf(0)
    {
    }

    static System instance_obj;

    // Graphics objects
    u_char  cur_buf; // 0 or 1

    // This sample uses split screen/4 view ports
    Display_buffer disp_buffs[2][NUM_VIEWPORTS];

    static const u_int FOG_NEAR = 300;
    static const u_int SCREEN_Z = 512;
};

#endif // end ifdef SYSTEM_H_INCLUDED
