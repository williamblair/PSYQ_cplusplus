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

    // Set up drawing for the current frame
    void start_frame();

    // Send the current frame to be drawn
    void end_frame();

    // stop the system; a new program
    // can be called with Exec() at this point,
    // so that's pretty cool
    void deinit();

private:

    // private singleton constructor
    System() :
        cur_buf(0)
    {
    }

    static System instance_obj;

    // Graphics objects
    DRAWENV draw[2]; // 2 drawing and display environments for double buffer
    DISPENV disp[2];
    u_char  cur_buf; // 0 or 1
};

#endif // end ifdef SYSTEM_H_INCLUDED
