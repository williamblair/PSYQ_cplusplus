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

int main(void)
{
    Cube              cube;
    System *          system      = System::get_instance();
    Pad               pad1;

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();
    system->init_3d();

    for (ever)
    {
        system->start_frame();

        pad1.read();
        if (pad1.is_held(PadUp))    cube.rotate(10,0,0);
        if (pad1.is_held(PadDown))  cube.rotate(-10,0,0);
        if (pad1.is_held(PadLeft))  cube.rotate(0,0,10);
        if (pad1.is_held(PadRight)) cube.rotate(0,0,-10);

        cube.draw();

        system->end_frame();
    }

    system->deinit();
    return 0;
}

