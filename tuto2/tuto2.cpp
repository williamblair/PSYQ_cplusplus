// implementation for tuto2 class

#include "tuto2.hpp"

void tuto2::run(void)
{
    POLY_G4 square; // square primitive to draw
    long dmy, flag;
    SVECTOR s_pos[4] = {
        {-128, -128, 0},
        { 128, -128, 0},
        {-128,  128, 0},
        { 128,  128, 0}
    }; // coordinate for each vertex on the square;

    int z_dist = 512; // how far in front of us the graphics are

    // rotation and translation matrices for 3d
    SVECTOR ang = {512, 512, 512}; // angle of the square
    VECTOR vec = {0, 0, 1024};   // position of the square
    MATRIX m; // resulting work matrix

    // initialize system,graphics,display
    initSystem();
    init3D(z_dist);
    initDB();

    // set up the square to draw
    setPolyG4(&square);
    setRGB0(&square, 255, 0, 0);
    setRGB1(&square, 0, 255, 0);
    setRGB2(&square, 0, 0, 255);
    setRGB3(&square, 255, 0, 255);

    // main loop
    while(1)
    {
        // clear the print stream
        FntFlush(-1);
        // print some info to the screen
        FntPrint("Pos: ");
        dumpXY4(&square);
        dumpVector("Angle: ", &ang);

        // get the current display buffer
        cdb = (cdb==&db[0]) ? &db[1] : &db[0];

        // apply the current display buffer
        PutDrawEnv(&cdb->draw);
        PutDispEnv(&cdb->disp);

        // read the controller; move/rotate square
        handlePad(&ang, &vec, &m);

        // rotate, translate, and apply the work matrix
        RotMatrix(&ang, &m);
        TransMatrix(&m, &vec);
        SetRotMatrix(&m);
        SetTransMatrix(&m);

        // apply 3d calculation to the square
        RotTransPers(&s_pos[0], (long*)&square.x0, &dmy, &flag);
        RotTransPers(&s_pos[1], (long*)&square.x1, &dmy, &flag);
        RotTransPers(&s_pos[2], (long*)&square.x2, &dmy, &flag);
        RotTransPers(&s_pos[3], (long*)&square.x3, &dmy, &flag);

        // clear the order table
        ClearOTag(cdb->ot, OT_LENGTH);

        // add the square to the order table
        AddPrim(cdb->ot, &square);

        // apply the order table
        DrawOTag(cdb->ot);

        // wait for the gpu to catch up
        DrawSync(0);
        VSync(0);
    }

}

// read the pad and change the angle/location of the square
void tuto2::handlePad(SVECTOR *ang, VECTOR *vec, MATRIX *m)
{
    // get controller information
    u_long padd = PadRead(1);

    // change the rotation angle
    if(padd & PADLup)    ang->vx += 32;
    if(padd & PADLdown)  ang->vx -= 32;
    if(padd & PADLleft)  ang->vy += 32;
    if(padd & PADLright) ang->vy -= 32;

    // change the scale
    if(padd & (PADL2|PADR2)) vec->vz += 8;
    if(padd & (PADL1|PADR1)) vec->vz -= 8;

    // auto rotate the square
    //ang->vx += 32;

    return;
}

void tuto2::initSystem(void)
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

void tuto2::init3D(int z_dist)
{
    // set the center of geometry drawing
    SetGeomOffset(160, 120);

    // set z distance
    // cannot be 0! doesn't init properly...
    SetGeomScreen(z_dist);
}

void tuto2::initDB(void)
{
    // set the drawing and display environment for the first buffer
    SetDefDrawEnv(&db[0].draw, 0, 0, 320, 240);
    SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);

    // set the drawing and display environment for the second buffer
    SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
    SetDefDispEnv(&db[1].disp, 0, 0, 320, 240);

    // set the drawing primitives as backgrounds and set their colors
    db[0].draw.isbg = 1;
    db[1].draw.isbg = 1;
    setRGB0(&db[0].draw, 60, 120, 120);
    setRGB0(&db[1].draw, 60, 120, 120);
}
