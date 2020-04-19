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
#include "Texture.h"

#define CUBESIZ 64

class Cube
{
public:

    Cube();

    // Adds each of the cube faces to the system OT
    //  (accesses the System singleton instance to add)
    void draw(MATRIX * transform_mat, SVECTOR * light_angle);

    // Rotate the amount given around each of the axes
    void rotate(int x, int y, int z);

    // Translate along each of the given axes
    void translate(int x, int y, int z);

    // Set the world matrix
//    void set_world_matrix(MATRIX * world);

#if 0    
    // Load texture data into VRAM
    void load_texture(
        u_long *texdata,  // pointer to texture data
        u_long *clutdata, // pointer to CLUT data (NULL if none)
        TEXTURE_BPP bpp,  // 0 - 16bit, 1 - 8bit, 2 - 4 bit
        int x,        // where in VRAM to load the texture
        int y,
        int w,        // size of the texture
        int h,
        int clutX,    // where to upload the clut data (if given)
        int clutY
    );

    // Set the size of the area from the texture to use
    // when drawing (e.g. loaded texture size is 256x256,
    //  but you only want to use 64x64 of it)
    void set_texture_size(u_int width, u_int height);
#endif

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


    // Lights (local or global?)
    static MATRIX color_mat; // light source local color matrix
    static MATRIX light_mat; // light source local light matrix

    // rotation angle for the light matrix
    static SVECTOR light_angle;
    // Light translation matrix
    MATRIX light_trans_mat;
    // The final calculated light matrix
    MATRIX light_mat_final;

    // Color of the cube (a color for each face)
    CVECTOR colors[6];

    // The primitives used for drawing
    POLY_F4 prims[6];

    // 3D transformation objects
    MATRIX  mat;
    SVECTOR angle;
    //VECTOR  translation;
    SVECTOR  translation;

    // Cube texture
    Texture texture;

    // The size of the portion of the texture to use when drawing
    u_int texture_clip_width;
    u_int texture_clip_height;
};

#endif // end ifdef CUBE_H_INCLUDED

