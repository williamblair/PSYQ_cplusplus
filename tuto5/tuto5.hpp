// header file for tutorial 5 class

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

#include <libgs.h>

#include <stdlib.h>

#ifndef TUTO5_HPP_INCLUDED
#define TUTO5_HPP_INCLUDED

#define OTSIZE (4096)

// display buffer class; this time with 6 Poly F4's to make a cube,
// one for each side
class DB
{
public:
    DRAWENV draw;
    DISPENV disp;
    u_long ot[OTSIZE];
    //POLY_F4 s[6];
    POLY_FT4 s[6]; // now its a textured primitive
};

class tuto5
{
public:
    // call this in main
    void run(void);

private:
    /************************/
    /**      VARIABLES     **/
    /************************/
    DB db[2]; // double buffer
    DB *cdb; // current buffer
    CVECTOR col[6]; // color of the cube

    // vectors/matrices for moving/scaling the cube
    MATRIX rottrans;
    SVECTOR ang;
    VECTOR vec;

    // light matrices and such
    MATRIX rotlgt;  // rotation/transformation matrix for light
    MATRIX light;   // the final calculated light matrix
    SVECTOR lgtang; // the light angle

    // texture variables
    u_short tpage, clut;

    /************************/
    /**      FUNCTIONS     **/
    /************************/
    void initSystem(void);
    void init3D(int x, int y, int z);
    void initDB(void);
    void initPrim(void);
    // notice this is now POLY_FT4 accordingly
    void addCubeF4(u_long *ot, POLY_FT4 *s, MATRIX *rottrans, CVECTOR *c);

    int padRead(void); // rotates/moves the matrix based on controller input
};

#endif // TUTO5_HPP_INCLUDED
