// blank main template for psyq c++

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

#include "System.h"
#include "Sprite.h"

#define ever ;;

POLY_G4 square; // square primitive to draw

SVECTOR square_coords[4] = {
    {-128, -128, 0}, // top left
    { 128, -128, 0}, // top right
    {-128,  128, 0}, // bottom left
    { 128,  128, 0}  // bottom right
};

// 4096 (ONE) = 360 degrees = 2*PI
SVECTOR angle        = {512, 512, 512}; // x,y,z rotation of square
VECTOR  world_coords = {0, 0, 1024}; 
MATRIX  m; // to store resulting tranformation matrix

// Initialize primitive values
static void init_square()
{
    setPolyG4(&square);
    setRGB0(&square, 255, 0,   0);
    setRGB1(&square, 0,   255, 0);
    setRGB2(&square, 0,   0,   255);
    setRGB3(&square, 255, 0,   255);
}

// Calculate world coordinates based on the square
//  (go from local to world coordinates)
static void calculate_world()
{
    RotMatrix(&angle, &m); // Create rotation matrix with ang and store in m
    TransMatrix(&m, &world_coords); // Translate matrix m with world coords

    // Apply the resulting matrix to the GTE
    SetRotMatrix(&m);
    SetTransMatrix(&m);
}

// Apply 3d perspective to the square
static void transform_square()
{
    static long dummy;
    static long flag;
    
    // transforms each of the square coords based on the currently set
    // rotation and translation matrix (done in calculate_world), then
    // stores the result in the square primitive to be drawn
    // 
    // From the documentation:
    //   long RotTransPers(
    //      SVECTOR *v0,    Pointer to vertex coordinate vector (input)
    //      long *sxy,      Pointer to screen coordinates 
    //      long *p,        Pointer to interpolated value 
    //      long *flag)     Pointer to flag
    // 
    //  After converting the coordinate vector v0 with a rotation matrix, the function performs 
    //  perspective transformation, and returns screen coordinates sx, sy.
    //  It also returns an interpolated value for depth cueing in p.
    RotTransPers(&square_coords[0], (long*)&square.x0, &dummy, &flag);
    RotTransPers(&square_coords[1], (long*)&square.x1, &dummy, &flag);
    RotTransPers(&square_coords[2], (long*)&square.x2, &dummy, &flag);
    RotTransPers(&square_coords[3], (long*)&square.x3, &dummy, &flag);
}

int main(void)
{
    System *          system      = System::get_instance();

    system->init();
    system->init_graphics();
    system->init_3d();

    init_square();

    for (ever)
    {
        FntPrint("Pos: ");
        dumpXY4(&square);
        dumpVector("Angle: ", &angle);
        
        system->start_frame();

        calculate_world();
        transform_square();

        system->add_prim(&square, 0);

        system->end_frame();
    }

    system->deinit();
    return 0;
}
