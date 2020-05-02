// Cube.cpp

#include "Cube.h"


Cube::Cube() :
    texture_clip_width(0),
    texture_clip_height(0)
{
    u_int i;

    // set cube colors
    colors[0].r = 255; colors[0].g = 0;     colors[0].b = 255;
    colors[1].r = 255; colors[1].g = 255;   colors[1].b = 0;
    colors[2].r = 255; colors[2].g = 0;     colors[2].b = 0;
    colors[3].r = 0;   colors[3].g = 255;   colors[3].b = 0;
    colors[4].r = 0;   colors[4].g = 0;     colors[4].b = 255;
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
    setVector(&translation, 0, 0, 0);

}

void Cube::draw(MATRIX * transform_mat, SVECTOR * light_angle)
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

    // Save current register
    PushMatrix();


    // apply our local translation
    //  assuming angle would be done here normally as well, but
    //  for this demo the angle is done globally
    RotTrans(&translation, (VECTOR*)transform_mat->t, &flg);

    // apply local rotation
    RotMatrix(&angle, transform_mat);

    // Calculate light
    RotMatrix(light_angle, &light_trans_mat);
    MulMatrix(&light_trans_mat, transform_mat);
    MulMatrix0(&light_mat, &light_trans_mat, &light_mat_final);

    // Apply light
    SetColorMatrix(&color_mat);
    SetLightMatrix(&light_mat_final);

    // Apply current transformations to GTE
    SetRotMatrix(transform_mat);
    SetTransMatrix(transform_mat);
    
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
            
            // if primitive is not back faced
            if (isomote > 0)
            {
                // If our depth is valid, add it to the order table
                if (otz > 0 && otz < OT_LEN)
                {
                    // Normal color based off light matrix
                    NormalColorCol(*normals_ptr,
                                    &colors[i],
                                    (CVECTOR*)&(prims[i].r0));

                    system->add_prim(&prims[i], otz);
                }
            }

            // Move to the next set of vertices/normals
            normals_ptr++;
            vertex_ptr += 4;
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

void Cube::translate(int x, int y, int z)
{
    translation.vx += x;
    translation.vy += y;
    translation.vz += z;
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


