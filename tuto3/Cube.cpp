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
        setRGB0(&prims[i], colors[i].r, colors[i].g, colors[i].b);
    }

    // default rotation and translation
    setVector(&angle, 0, 0, 0);
    setVector(&translation, 5, 0, 512);
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

    // Set GTE rotation and translation
    RotMatrix(&angle, &mat);
    TransMatrix(&mat, &translation);
    SetRotMatrix(&mat);
    SetTransMatrix(&mat);

    // The GTE has one matrix register; its best to save it
    PushMatrix();
    
        for (i=0; i<6; ++i)
        {
            
            // translate from local coordinates to screen
            // coordinates using RotAverageNclip4()
            // otz represents 1/4 value of the average of z
            // value of each vertex
            isomote = RotAverageNclip4(vertex_ptr[0], // SVECTOR *v0, v1, v2, v3
                                       vertex_ptr[1], 
                                       vertex_ptr[2], 
                                       vertex_ptr[3], 
                                        
                                      (long*)&prims[i].x0, 
                                      (long*)&prims[i].x1, // long *sxy0, sxy1
                                      (long*)&prims[i].x3, // three then two intentionally
                                                               // the vertices order must be screwed up
                                      (long*)&prims[i].x2, // long *sxy2, sxy3
                                        
                                      &p, &otz, &flg);
            
            // if primitive is back faced, skip it
            if (isomote <= 0)
            {
                continue;
            }

            // If our depth is valid, add it to the order table
            if (otz > 0 && otz < OT_LEN)
            {
                system->add_prim(&prims[i], otz);
            }

            // Update which vertices we're working with
            vertex_ptr += 4;
        } 


    // Restore the matrix register
    PopMatrix();
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
	&Cube::N5, &Cube::N0, &Cube::N4, &Cube::N1, &Cube::N3, &Cube::N2
};


