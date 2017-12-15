// header for tuto2 class
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

#ifndef TUTO2_H_INCLUDED_
#define TUTO2_H_INCLUDED_

#define OT_LENGTH 10

// display buffer class
class DB
{
public:
    DRAWENV draw;
    DISPENV disp;
    u_long ot[OT_LENGTH];
};

class tuto2
{
public:
    void run(void);
private:
    /****************************/
    /**       VARIABLES        **/
    /****************************/
    DB db[2]; // two display buffers for double buffering
    DB *cdb; // current dat buffers; flips every frame


    /****************************/
    /**       FUNCTIONS        **/
    /****************************/
    void initSystem(void); // set up the display
    void init3D(int z_dist); // set up geometry transformation and z coords
    void initDB(void); // init the display buffers

    // read the pad and change the angle/location of the square
    void handlePad(SVECTOR *ang, VECTOR *vec, MATRIX *m);
};
#endif // TUTO2_H_INCLUDED_
