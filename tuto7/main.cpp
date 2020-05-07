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

// texture pattern in bgtex.c
extern u_long bgtex[];

// To rotate and translate the entire world
SVECTOR world_angle = {0,0,0};
VECTOR world_translate = {0,0,1024};
MATRIX world_transform;

// To rotate and translate each cube
MATRIX local_transform;

// Light matrices
SVECTOR lgtang = {1024,-512,1024};


#define ever ;;

int main(void)
{
    int               i;
    static const int  ncubes      = 100;
    Cube              cubes[ncubes];
    System *          system      = System::get_instance();
    Pad               pad1;

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();
    system->init_3d();

    // Init cube locations
    for (i = 0; i < ncubes; ++i)
    {
        int x = (rand() % 400) - 200;
        int y = (rand() % 400) - 200;
        int z = (rand() % 400) - 200;
        cubes[i].translate(x,y,z);
    }
    
    for (ever)
    {
        system->start_frame();

        
        pad1.read();

        if (pad1.is_held(PadUp))    world_translate.vz += 10;
        if (pad1.is_held(PadDown))  world_translate.vz -= 10;

        if (pad1.is_held(PadR1)) world_angle.vy += 10;
        if (pad1.is_held(PadL1)) world_angle.vy -= 10;

        if (pad1.is_held(PadR2)) lgtang.vz += 10;
        if (pad1.is_held(PadL2)) lgtang.vz -= 10;


        FntPrint("World Translate: %d,%d,%d\n", world_translate.vx, 
                                              world_translate.vy,
                                              world_translate.vz);
        FntPrint("World Angle: %d,%d,%d\n", world_angle.vx, 
                                              world_angle.vy,
                                              world_angle.vz);
        FntPrint("Light Angle: %d,%d,%d", lgtang.vx,
                                           lgtang.vy,
                                           lgtang.vz);

        // Init the world matrix
        RotMatrix(&world_angle, &world_transform);
        TransMatrix(&world_transform, &world_translate);

        // Initialize the local screen matrix
        // MulMatrix2 stores the result in the second arg; destroys current rotation mat
        MulMatrix2(&world_transform, &local_transform);

        // TODO - scale the matrix
        //
    
        // Set matrix for all objects
        SetRotMatrix(&world_transform);
        SetTransMatrix(&world_transform);

        for (i = 0; i < ncubes; ++i)
        {
            // test individual rotation
            cubes[i].rotate(10, 0, 5);

            // local transform will be modified to account
            // for our cube translation
            cubes[i].draw(&local_transform, &lgtang);
        }

        system->end_frame();
    }

    system->deinit();
    return 0;
}

