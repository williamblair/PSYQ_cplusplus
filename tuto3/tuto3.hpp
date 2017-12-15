// header file for tutorial 3 class

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <stdlib.h>

#ifndef TUTO3_HPP_INCLUDED
#define TUTO3_HPP_INCLUDED

#define OTSIZE (4096)

// display buffer class; this time with 6 Poly F4's to make a cube,
// one for each side
class DB
{
public:
    DRAWENV draw;
    DISPENV disp;
    u_long ot[OTSIZE];
    POLY_F4 s[6];
};

class tuto3
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

    /************************/
    /**      FUNCTIONS     **/
    /************************/
    void initSystem(void);
    void init3D(int x, int y, int z);
    void initDB(void);
    void addCubeF4(u_long *ot, POLY_F4 *s, MATRIX *rottrans);

    int padRead(void); // rotates/moves the matrix based on controller input
};

#endif // TUTO3_HPP_INCLUDED
