// implementation file for tutorial 4

#include "tuto5.hpp"

//#include "..\global_src\CUBE.C"
//#include "..\global_src\BGTEX.C"
#include "test.h"
//#include "BMPTEST.H"

// cube data
/*
 *	cube vertex database */
#define CUBESIZ	196		/* cube size*/

/* vertex */
static SVECTOR P0 = {-CUBESIZ/2,-CUBESIZ/2,-CUBESIZ/2,0};
static SVECTOR P1 = { CUBESIZ/2,-CUBESIZ/2,-CUBESIZ/2,0};
static SVECTOR P2 = { CUBESIZ/2, CUBESIZ/2,-CUBESIZ/2,0};
static SVECTOR P3 = {-CUBESIZ/2, CUBESIZ/2,-CUBESIZ/2,0};

static SVECTOR P4 = {-CUBESIZ/2,-CUBESIZ/2, CUBESIZ/2,0};
static SVECTOR P5 = { CUBESIZ/2,-CUBESIZ/2, CUBESIZ/2,0};
static SVECTOR P6 = { CUBESIZ/2, CUBESIZ/2, CUBESIZ/2,0};
static SVECTOR P7 = {-CUBESIZ/2, CUBESIZ/2, CUBESIZ/2,0};

/* normal */
// ONE is defined in libgte.h as 4096; as that
// equals 360 degrees for the gte
static SVECTOR N0 = { ONE,   0,    0, 0,};
static SVECTOR N1 = {-ONE,   0,    0, 0,};
static SVECTOR N2 = {0,    ONE,    0, 0,};
static SVECTOR N3 = {0,   -ONE,    0, 0,};
static SVECTOR N4 = {0,      0,  ONE, 0,};
static SVECTOR N5 = {0,      0, -ONE, 0,};

// vertices list
static SVECTOR	*v[6*4] = {
	&P0,&P1,&P2,&P3,
	&P1,&P5,&P6,&P2,
	&P5,&P4,&P7,&P6,
	&P4,&P0,&P3,&P7,
	&P4,&P5,&P1,&P0,
	&P6,&P7,&P3,&P2,
};

// normal list
static SVECTOR	*n[6] = {
	&N5, &N0, &N4, &N1, &N3, &N2,
};
// end cube data

// light data

// light source local color matrix
static MATRIX cmat = {
    // light source 0, 1, and 2
    ONE*3/4, 0, 0, // r
    ONE*3/4, 0, 0, // g
    ONE*3/4, 0, 0  // b
};

// light vector (local light matrix)
static MATRIX lgtmat = {
    // x, y, z
    ONE, ONE, ONE, // light source # 0
    0, 0, 0,       //              # 1
    0, 0, 0        //              # 2
};

// end light data

void tuto5::run(void)
{
    // apply world rotation/transformation
    //MATRIX rottrans;
    //SVECTOR ang = {0,0,0};
    //VECTOR vec = {0,0,512};
    setVector(&ang, 0,0,0);
    setVector(&vec, 0,0,512);
    setVector(&lgtang, 1024, -512, 1024);

    // initialize stuff
    initSystem();
    init3D(320, 240, 512); // x, y, z
    initDB();

    // set the local color matrix
    SetColorMatrix(&cmat);

    //GsIMAGE image;
    //GsGetTimInfo(TEST_TIM, &image);
    //GsGetTimInfo(BMPTEST_TIM, &image);

    // load the texture and CLUT
    //clut = LoadClut(bgtex, 0, 480);
    //tpage = LoadTPage(bgtex + 0x80, 0, 0, 640, 0, 256, 256);
    clut = LoadClut(testHeader, 0, 480);
    tpage = LoadTPage(testHeader + 0x80, 0, 0, 640, 0, 1024, 1024);
    //tpage = LoadTPage(testHeader + 14, 0, 0, 640, 0, 256, 256);

    // test...
    //clut = LoadClut(image.clut, 0, 480);
    //tpage = LoadTPage(image.pixel, 0, 0, 320, 0, 256, 256);
    //tpage = LoadTPage(
    //                  image.pixel, // unsigned long *pixel
    //                  0,           // bit depth - (0=4bit, 1=8bit, 2=16 bit)
    //                  0,           // semitransparency rate
    //                  640, 0,       // dest frame buffer address
    //                  256, 256
    //                  );
    //LoadTPage()

    // set primitive parameters on buffer
    initPrim();

    // set the color of the cube
    for(int i=0; i<6; i++)
    {
        // set surface colors (controlled by the light)
        col[i].cd = db[0].s[0].code; // code
        //col[i].r = rand();
        //col[i].g = rand();
        //col[i].b = rand();

        col[i].r = 0x80;
        col[i].g = 0x80;
        col[i].b = 0x80;

        // we removed setRGB as this is controlled in addCubeF4 now
    }

    // necessary?
    SetDispMask(1);

    /*
	 * When using interlace mode, there is no need to changing
	 * draw/display environment for every frames because the same
	 * same area is used for both drawing and displaying images.
	 * Therefore, the environment should be set only at the first time.
	 * Even in this case, 2 primitive buffers are needed since drawing
	 * process runs parallel with CPU and GPU. */
	PutDrawEnv(&db[0].draw);	/* set up rendering environment*/
	PutDispEnv(&db[0].disp);	/* set up display environment*/

	// initialize the angle and transformation
	//SVECTOR ang = {0,0,0};
	//VECTOR vec = {0,0,512};

    // main loop
    while(padRead() != -1)
    {
        // swap display buffer
        cdb = (cdb == &db[0]) ? &db[1] : &db[0];

        /* clear OT.
		 * ClearOTagR() clears OT as reversed order. This is natural
		 * for 3D type application, because OT pointer to be linked
		 * is simply related to Z value of the primivie. ClearOTagR()
		 * is faster than ClearOTag because it uses hardware DMA
		 * channel to clear. */
		ClearOTagR(cdb->ot, OTSIZE);

		// calculate matrix for light source
		MulMatrix0(&lgtmat, &rotlgt, &light);
		SetLightMatrix(&light);


		// add the cube to the order table, this time
		// with the given colors adjusting for light
		// the addCubeF4 function is different now too
		addCubeF4(cdb->ot, cdb->s, &rottrans, col);

		// When using interlaced single buffer, all
		// drawing has to be finished in 1/60 sec.
		// so we have to reset the drawing procedure
		// at the timing of VSync by calling
		// ResetGraph(1) instead of DrawSync(0)
		VSync(0);
		ResetGraph(1);

		// clear the background
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		// draw the order table
		// since ClearOTagR clears the OT in
		// reversed order, the top pointer of
		// the table is ot[OTSIZE-1]. Notice that
		// drawing start point is not ot[0] but
		// ot[SIZE-1]
		DrawOTag(cdb->ot+OTSIZE-1);
		FntFlush(-1);
    }
    DrawSync(0);
    return;
}

void tuto5::initSystem(void)
{
    RECT rect; // the screen to draw on

    ResetGraph(0);  // initialize renderer
    InitGeom();     // initialize geometry library
    DecDCTReset(0); // initialize decompresser

    SetGraphDebug(0); // set graphics debug mode

    PadInit(0); // initialize controller library

    // are these necessary?
    setRECT(&rect, 0, 0, 256, 240);
    ClearImage(&rect, 0, 0, 0); // set bg color

    SetDispMask(1); // turn on the display

    // load the system font
    FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));
}

// x,y - 2d screen coordinates offset
// z - distance to drawing
void tuto5::init3D(int x, int y, int z)
{
    // set the center of geometry drawing
    SetGeomOffset(x, y);

    // set z distance
    // cannot be 0! doesn't init properly...
    SetGeomScreen(z);
}

// this time we're using 640x480 interlaced resolution!
void tuto5::initDB(void)
{
    // notice everything happens at 0,0,640,480
    SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
    SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
    SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
    SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

    // initialize the primitive
    for(int i=0; i<6; i++)
    {
        // these are FT4s now
        SetPolyFT4(&db[0].s[i]);
        SetPolyFT4(&db[1].s[i]);
    }
}

void tuto5::addCubeF4(u_long *ot, POLY_FT4 *s, MATRIX *rottrans, CVECTOR *c)
{
    int i;
    long p, otz, opz, flg;
    int isomote;
    SVECTOR **vp; // vertex pointer
    SVECTOR **np; // normal pointer

    // GTE has one matrix register, so best to save it
    PushMatrix();

    // set rotation and translation matrices
    SetRotMatrix(rottrans);
    SetTransMatrix(rottrans);

    // if the primitive is not back faced, then put it into OT
    vp = v; // vp = vertex pointer

    np = n; // n declared above

    // notice we increase the normal pointer and color now too
    for(i=0; i<6; i++, s++, vp += 4, c++, np++)
    {
        // translate from local coordinates to screen
        // coordinates using RotAverageNclip4()
        // otz represents 1/4 value of the average of z
        // value of each vertex
        isomote = RotAverageNclip4(vp[0], vp[1], vp[2], vp[3], // SVECTOR *v0, v1, v2, v3
                                   (long*)&s->x0, (long*)&s->x1, // long *sxy0, sxy1
                                   (long*)&s->x3, (long*)&s->x2, // long *sxy2, sxy3
                                   &p, &otz, &flg); // not sure what these do...
        // if not surface, do nothing
        if(isomote <= 0) continue;

        // otherwise add the primitive to the ot
        // assumes length of ot is 4096
        if(otz > 0 && otz < 4096) {

            // set the color
            NormalColorCol(*np, c, (CVECTOR *)&s->r0);

            // OT is assumed to be cleared in reverse order
            // in this condition, the OT pointer is
            // calculated as simply ot+otz
            AddPrim(ot+otz, s);
        }
    }

    // restore the matrix register
    PopMatrix();

    return;
}

int tuto5::padRead(void)
{

    // read the controller
    u_long padd = PadRead(1);

    // return status
    int ret = 0;

    // quit
    if(padd & PADselect) ret = -1;

    // change light rotation angle
    if(padd & PADLup)    lgtang.vx += 32;
    if(padd & PADLdown)  lgtang.vx -= 32;
    if(padd & PADLleft)  lgtang.vy += 32;
    if(padd & PADLright) lgtang.vy -= 32;

    // change rotation angle
    if(padd & PADRup) ang.vz += 32;
    if(padd & PADRdown) ang.vz -= 32;
    if(padd & PADRleft) ang.vy += 32;
    if(padd & PADRright) ang.vy -= 32;

    // if the angles have gone past 360deg / 4096gte coords,
    // reset them
    ang.vz %= 4096;
    ang.vy %= 4096;

    // change distance from screen
    if(padd & (PADL2|PADR2)) vec.vz += 8;
    if(padd & (PADL1|PADR1)) vec.vz -= 8;

    // calculate and apply light matrix
    RotMatrix(&lgtang, &rotlgt);
    MulMatrix(&rotlgt, &rottrans);

    // calculate and apply matrix
    RotMatrix(&ang, &rottrans);
    TransMatrix(&rottrans, &vec);

    SetRotMatrix(&rottrans);
    SetTransMatrix(&rottrans);

    FntPrint("tuto4: lighting angle: (%d,%d,%d)\n",
             lgtang.vx, lgtang.vy, lgtang.vz);

    return ret;
}

void tuto5::initPrim(void)
{
    // textured 4 point polygon declared (one for each cube side)
    for(int i=0; i<6; i++)
    {
        // two times - one for each buffer
        setPolyFT4(&(db[0].s[i]));
        setUV4(&(db[0].s[i]), 0, 0, 0, 64, 64, 0, 64, 64);
        setRGB0(&(db[0].s[i]), 128, 128, 128);
        db[0].s[i].tpage = tpage;
        db[0].s[i].clut = clut;

        setPolyFT4(&(db[1].s[i]));
        setUV4(&(db[1].s[i]), 0, 0, 0, 64, 64, 0, 64, 64);
        setRGB0(&(db[1].s[i]), 128, 128, 128);
        db[1].s[i].tpage = tpage;
        db[1].s[i].clut = clut;
    }

    return;
}
