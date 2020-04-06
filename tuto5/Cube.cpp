// Cube.cpp

#include "Cube.h"


Cube::Cube() :
    texture_clip_width(0),
    texture_clip_height(0)
{
    u_int i;

    // set cube colors
    //  In this tutorial they are all middle brightnes for the texture
    colors[0].r = 128; colors[0].g = 128;   colors[0].b = 128;
    colors[1].r = 128; colors[1].g = 128;   colors[1].b = 128;
    colors[2].r = 128; colors[2].g = 128;   colors[2].b = 128;
    colors[3].r = 128; colors[3].g = 128;   colors[3].b = 128;
    colors[4].r = 128; colors[4].g = 128;   colors[4].b = 128;
    colors[5].r = 128; colors[5].g = 128;   colors[5].b = 128;

    // initialize prim properties
    for (i=0; i<6; ++i)
    {
        SetPolyFT4(&prims[i]);

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

    // Calculate the light transformation matrix matrix (light_trans_mat)
    // (move from local coordinates to the current object world coordinates)
    //  multiplied by the object transformation matrix
    RotMatrix(&light_angle, &light_trans_mat);
    MulMatrix(&light_trans_mat, &mat); // the result is saved in the first matrix

    // Apply the light transformation matrix to the actual light matrix
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
            
            // if primitive is not back faced
            if (isomote > 0)
            {
                // If our depth is valid, add it to the order table
                if (otz > 0 && otz < OT_LEN)
                {
                    // Additional step now from previous tut (3)
                    // Set the color of the primitives
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

void Cube::load_texture(
        u_long *texdata,  // pointer to texture data
        u_long *clutdata, // pointer to CLUT data (NULL if none)
        TEXTURE_BPP bpp,  // 0 - 16bit, 1 - 8bit, 2 - 4 bit
        int x,        // where in VRAM to load the texture
        int y,
        int w,        // size of the texture
        int h,
        int clutX,    // where to upload the clut data (if given)
        int clutY)
{
    u_int i;

    // Forward the data to the texture
    texture.load(texdata,
                 clutdata,
                 bpp,
                 x, y,
                 w, h,
                 clutX,
                 clutY);

    // Update the default texture size (if not initialized)
    if (texture_clip_width == 0)
    {
        texture_clip_width = texture.get_width();
    }
    if (texture_clip_height == 0)
    {
        texture_clip_height = texture.get_height();
    }

    // Update the primitives UV coordinates
    set_texture_size(texture_clip_width,
                     texture_clip_height);
}

void Cube::set_texture_size(u_int width, u_int height)
{
    u_int i;

    texture_clip_width = width;
    texture_clip_height = height;

    // Update the primitives
    for (i=0; i<6; ++i)
    {
        // assuming UV coordinates 0,0 and width/height 
        // same as texture for now
        setUVWH(&prims[i],
                0,0,        // TODO - have UV customizable
                texture_clip_width, 
                texture_clip_height);

        prims[i].tpage = texture.get_texture_page_id();
        prims[i].clut = texture.get_clut_id();
    }
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

