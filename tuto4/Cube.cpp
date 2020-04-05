// Cube.cpp

#include "Cube.h"


Cube::Cube()
{
    u_int i;

    // set cube colors
    colors[0].r = 255; colors[0].g = 0;     colors[0].b = 0;
    colors[1].r = 0;   colors[1].g = 255;   colors[1].b = 0;
    colors[2].r = 0;   colors[2].g = 0;     colors[2].b = 255;
    colors[3].r = 255; colors[3].g = 0;     colors[3].b = 255;
    colors[4].r = 255; colors[4].g = 255;   colors[4].b = 0;
    colors[5].r = 0;   colors[5].g = 255;   colors[5].b = 255;

    // initialize prim properties
    for (i=0; i<6; ++i)
    {
        SetPolyF4(&prims[i]);

        // This is now done in cube drawing calculation, where color is
        // based off of light shading
        setRGB0(&prims[i], colors[i].r, colors[i].g, colors[i].b);

        // surface color code controlled by light?
        colors[i].cd = prims[i].code;
    }

    // default rotation and translation
    setVector(&angle, 0, 0, 0);
    setVector(&translation, 5, 0, 512);

    // Light angle
    setVector(&light_angle, 1024, -512, 1024);
}

void Cube::draw()
{
    u_int i;
    int isomote;
    long p, otz, opz, flg;

    // System instance so we can add prims to the order table
    System * system = System::get_instance();

    // Current cube vertex
    SVECTOR  ** vertex_ptr = vertices;

    // Current normal coordinates
    SVECTOR  ** normals_ptr = normals;


    // The GTE has one matrix register; its best to save it
    PushMatrix();
    

    // Set GTE rotation and translation
    RotMatrix(&angle, &mat);
    TransMatrix(&mat, &translation);

    // Calculate the light translation matrix (light_trans_mat)
    //  multiplied by the object transformation matrix
    RotMatrix(&light_angle, &light_trans_mat);
    MulMatrix(&light_trans_mat, &mat); // the result is saved in the first matrix

    // Calculate and set the light matrix
    // first two args are input, last is output
    //  from docs: "The function destroys the constant rotation matrix."
    //  This might be why the Set* matrices need to happen AFTER this
    MulMatrix0(&light_mat, &light_trans_mat, &light_mat_final);
    
    // Set each of the matrices in the GTE
    // This should happen LAST before starting to draw
    //      Not sure why, just based on experimenting...
    SetColorMatrix(&color_mat);
    SetLightMatrix(&light_mat_final);
    SetRotMatrix(&mat);
    SetTransMatrix(&mat);

    
        for (i=0; i<6; i++)
        {
            
            // translate from local coordinates to screen
            // coordinates using RotAverageNclip4()
            // otz represents 1/4 value of the average of z
            // value of each vertex
            isomote = RotAverageNclip4(vertex_ptr[0], // SVECTOR *v0, v1, v2, v3
                                       vertex_ptr[1], 
                                       vertex_ptr[2], 
                                       vertex_ptr[3], 
                                        
                                      (long*)&(prims[i].x0), 
                                      (long*)&(prims[i].x1), // long *sxy0, sxy1
                                      (long*)&(prims[i].x3), // three then two intentionally
                                                               // the vertices order must be screwed up
                                      (long*)&(prims[i].x2), // long *sxy2, sxy3
                                        
                                      &p, &otz, &flg);
            
            // Update which vertices we're working with
            vertex_ptr += 4;
            
            // if primitive is back faced, skip it
            if (isomote <= 0)
            {
                normals_ptr++;
                continue;
            }

            // If our depth is valid, add it to the order table
            if (otz > 0 && otz < OT_LEN)
            {
                // Additional step now from previous tut (3)
                // Set the color of the primitives
                NormalColorCol(*normals_ptr, &colors[i], (CVECTOR*)&(prims[i].r0));

                system->add_prim(&prims[i], otz);

                normals_ptr++;
            }

        } 


    // Restore the matrix register
    PopMatrix();
}

void Cube::rotate(int x, int y, int z)
{
    angle.vx += x;
    angle.vy += y;
    angle.vz += z;
}


// Cube data
SVECTOR Cube::P0 = {-CUBESIZ/2,-CUBESIZ/2,-CUBESIZ/2,0};
SVECTOR Cube::P1 = { CUBESIZ/2,-CUBESIZ/2,-CUBESIZ/2,0};
SVECTOR Cube::P2 = { CUBESIZ/2, CUBESIZ/2,-CUBESIZ/2,0};
SVECTOR Cube::P3 = {-CUBESIZ/2, CUBESIZ/2,-CUBESIZ/2,0};

SVECTOR Cube::P4 = {-CUBESIZ/2,-CUBESIZ/2, CUBESIZ/2,0};
SVECTOR Cube::P5 = { CUBESIZ/2,-CUBESIZ/2, CUBESIZ/2,0};
SVECTOR Cube::P6 = { CUBESIZ/2, CUBESIZ/2, CUBESIZ/2,0};
SVECTOR Cube::P7 = {-CUBESIZ/2, CUBESIZ/2, CUBESIZ/2,0};

// ONE is defined in libgte.h as 4096; as that
// equals 360 degrees for the gte
SVECTOR Cube::N0 = { ONE,   0,    0, 0,};
SVECTOR Cube::N1 = {-ONE,   0,    0, 0,};
SVECTOR Cube::N2 = {0,    ONE,    0, 0,};
SVECTOR Cube::N3 = {0,   -ONE,    0, 0,};
SVECTOR Cube::N4 = {0,      0,  ONE, 0,};
SVECTOR Cube::N5 = {0,      0, -ONE, 0,};


// vertices list
SVECTOR	* Cube::vertices[6*4] = {
	&Cube::P0,&Cube::P1,&Cube::P2,&Cube::P3,
	&Cube::P1,&Cube::P5,&Cube::P6,&Cube::P2,
	&Cube::P5,&Cube::P4,&Cube::P7,&Cube::P6,
	&Cube::P4,&Cube::P0,&Cube::P3,&Cube::P7,
	&Cube::P4,&Cube::P5,&Cube::P1,&Cube::P0,
	&Cube::P6,&Cube::P7,&Cube::P3,&Cube::P2,
};

// normal list
SVECTOR	* Cube::normals[6] = {
	&Cube::N5, &Cube::N0, &Cube::N4, &Cube::N1, &Cube::N3, &Cube::N2,
};

// Light source local color matrix
MATRIX Cube::color_mat = {
    // Light source 0, 1, and 2
    ONE*3/4, 0, 0, // r
    ONE*3/4, 0, 0, // g
    ONE*3/4, 0, 0, // b
};

// Light source local light matrix
MATRIX Cube::light_mat = {
    // x, y, z
    ONE, ONE, ONE, // light source #0
    0, 0, 0,       //              #1
    0, 0, 0        //              #2
};

// Light angle
SVECTOR Cube::light_angle;

