// header for tut1 class
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

#ifndef TUT1_HPP_INCLUDED
#define TUT1_HPP_INCLUDED

// basic data type to represent a display buffer,
// will want two for double buffering
class DB
{
public:
    DRAWENV draw; // drawing environment
    DISPENV disp; // display environment
    u_long ot[9]; // ordering table
    SPRT_16 ball[8][8][8]; // sprite buffer,
                           // Z direction: 8,
                           // X direction: 8,
                           // Y direction: 8
};

class tuto1
{

public:
    tuto1(void); // constructor
    void run(void); // called in main.cpp to run everything

private:

    /******************************************************/
    /**                 VARIABLES                        **/
    /******************************************************/
    DB db[2];    // primitive double buffer
    DB *cdb;     // current display buffer
    SPRT_16 *bp; // current sprite we're working on
    int ox, oy;  // order x, order y? (order table?)

    int dx, dy;   // TODO - comment me

    int depth;   // keeps track of the depth of the ordering table
    int x, y;    // location of the current sprite?

    int ix, iy;       // TODO - comment me

    /******************************************************/
    /**                 FUNCTIONS                        **/
    /******************************************************/
    void init_prim(DB &db); // initialize a display buffer
    int pad_read(int *dx, int *dy); // read pad and adjust dx, dy based on it

    void tuto1_main(void); // main loop for the program

    // initialize and shutdown psx
    // internals
    void startSystem(int mode);
    void stopSystem(void);
};

#endif // TUT1_HPP_INCLUDED
