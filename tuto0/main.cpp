// rewriting tutorial 1 in c++

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

#include "System.h"
#include "Sprite.h"

#define ever ;;

int main(void)
{
    Sprite_textured   sprite;
    System *          system      = System::get_instance();


    system->init();
    system->init_graphics();

    for (ever)
    {
        system->start_frame();

        // Draw

        system->end_frame();
    }

    system->deinit();
    return 0;
}
