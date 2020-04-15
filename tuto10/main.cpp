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

        system->end_frame();
    }

    system->deinit();
    return 0;
}

