// blank main template for psyq c++

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

#include "System.h"
#include "Sprite.h"
#include "Cube.h"

#define ever ;;

int main(void)
{
    Cube              cube;
    System *          system      = System::get_instance();

    system->init();
    system->init_graphics();
    system->init_3d();

    for (ever)
    {
        system->start_frame();

        cube.draw();

        system->end_frame();
    }

    system->deinit();
    return 0;
}

