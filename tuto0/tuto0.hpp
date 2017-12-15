#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

#include <stdio.h>

#ifndef TUTO0_HPP_INCLUDED
#define TUTO0_HPP_INCLUDED

#define DT	4	/* sprite moving unit*/

class tuto0
{
public:
    void run(void); // runs the main loop

private:
    // code specific to this tutorial
    // (replaces tuto0 from psyq tutorial)
    void main_tuto(void);

    // fills tpage and clut with texture page and
    // color look up table information, contained in
    // MATTEX.C
    void load_texture4(u_short tpage[4], u_short clut[4]);

    // system functions
    void startSystem(int mode);
    void stopSystem(void);
};

#endif // TUTO0_HPP_INCLUDED
