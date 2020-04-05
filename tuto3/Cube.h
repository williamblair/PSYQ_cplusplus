// Cube.h

#ifndef CUBE_H_INCLUDED
#define CUBE_H_INCLUDED

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <stdlib.h>

#include "System.h"

#define CUBESIZ 128

class Cube
{
public:

    Cube();

    // Adds each of the cube faces to the system OT
    //  (accesses the System singleton instance to add)
    void draw();

    // Rotate the amount given around each of the axes
    void rotate(int x, int y, int z);

private:
    
    // cube points
    static SVECTOR P0;
    static SVECTOR P1;
    static SVECTOR P2;
    static SVECTOR P3;
    static SVECTOR P4;
    static SVECTOR P5;
    static SVECTOR P6;
    static SVECTOR P7;

    // cube normals vectors
    static SVECTOR N0;
    static SVECTOR N1;
    static SVECTOR N2;
    static SVECTOR N3;
    static SVECTOR N4;
    static SVECTOR N5;
    static SVECTOR N6;
    static SVECTOR N7;

    // vertices - pointers to coordinates
    static SVECTOR * vertices[6 * 4];

    // normals - pointers to normals vectors
    static SVECTOR * normals[6];

    // Color of the cube (a color for each face)
    CVECTOR colors[6];

    // The primitives used for drawing
    POLY_F4 prims[6];

    // 3D transformation objects
    MATRIX  mat;
    SVECTOR angle;
    VECTOR  translation;
};

#endif // end ifdef CUBE_H_INCLUDED

