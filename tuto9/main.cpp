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
    int               i;
    u_char            r,g,b;
    System *          system      = System::get_instance();
    Pad               pad1;
    POLY_F4           testpoly[4];

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();

    
    for (ever)
    {
        pad1.read();

        system->set_dispenv();


        // Draw each viewport
        for (i = 0; i < NUM_VIEWPORTS; ++i)
        {
            system->start_frame(i);

            FntPrint("Viewport %d\n", i);

            system->end_frame(i);
            
            // DrawSync for each viewport is probably slow but
            //  otherwise all FntPrints show up in the last viewport
            FntFlush(-1);
            DrawSync(0);
        }
        
        VSync(0);
    }

    system->deinit();
    return 0;
}

