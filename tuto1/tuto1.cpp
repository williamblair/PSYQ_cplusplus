// implementation for tutorial 1 class

#include "tuto1.hpp"

tuto1::tuto1(void){
    dx=0; dy=0;
}

void tuto1::run(void)
{
    // init psx internals
    startSystem(0);

    // main loop
    tuto1_main();

    // shutdown psx internals
    stopSystem();

    return;
}

// the main loop of the program
void tuto1::tuto1_main(void)
{
    // load the bios font and set its location
    // to print on the screen
    FntLoad(960, 256); // load the font into framebuffer location 960,256
    SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512)); // args: x,y,width,height,autoclear,max chars

    // initialize the double buffered display env
    SetDefDrawEnv(&db[0].draw, 0, 0, 320, 240); // args: DRAW *env, x, y, width, height
    SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
    SetDefDispEnv(&db[0].disp, 0, 240, 320, 240); // args: DISP *env, x, y, width, height
    SetDefDispEnv(&db[1].disp, 0, 0, 320, 240);

    // init primitive members of each buffer
    init_prim(db[0]);
    init_prim(db[1]);

    // Display enable
    SetDispMask(1);

    // main loop
    while(pad_read(&dx, &dy) == 0)
    {
        // switch to the other display buffer
        cdb = (cdb==db) ? db+1 : db;

        // clear the ordering table (8 entry)
        ClearOTag(cdb->ot, 8);

        // calculate the position of sprites
        for(depth=0; depth<8; depth++)
        {
            // far sprites move slowly, near sprites move fast
            oy = 56 + dy*(depth+1);

            // left upper corner (Y)
            ox = 96+dx*(depth+1);

            for(iy=0, y = oy; iy<8; iy++, y+=16){
                for(ix=0, x=ox; ix<8; ix++, x+=16)
                {
                    // get the current ball sprite pointer
                    bp = &cdb->ball[depth][iy][ix];

                    // set upper left corner of the sprite
                    setXY0(bp, x, y);

                    // add the primitive to the OT
                    AddPrim(&cdb->ot[depth], bp);
                }
            }
        }

        // wait for the gpu to catch up
        DrawSync(0);

        // wait for vertical blank
        VSync(0);

        // register the current display buffer with the system
        PutDrawEnv(&cdb->draw);
        PutDispEnv(&cdb->disp);

        // draw to the screen
        DrawOTag(cdb->ot);

        // debug print to the screen
        FntPrint("tuto1: OT\n");

        // clear the output stream
        FntFlush(-1);
    }

    DrawSync(0); // wait for the gpu to catch up
    return;
}

void tuto1::init_prim(DB &db)
{
    // 16x16 ball texture pattern
    extern u_long ball16x16[];

    // ball CLUT (16 colors x 32)
    extern u_long ballclut[][8];

    SPRT_16 *bp; // current ball pointer
    u_short clut; // color look up table for the ball pointer
    int depth, x, y; // how far back the ball is stacked, x, and y position

    // translate the 4bit texture pattern to (640, 0) on the
    // framebuffer and set the default texture page
    db.draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16); // args: u_long* pix, int tp, int abr, int x, int y, int w, int h

    // background clear enable and set bg color
    db.draw.isbg = 1;
    setRGB0(&db.draw, 60, 120, 120); // RGB

    // initialize 8x8x8=256 sprites at this point
    // CLUT of each texture is changed according to the sprite's depth
    for(depth=0; depth < 8; depth++)
    {
        // load CLUT on each frame buffer (notice the CLUT address)
        clut = LoadClut(ballclut[depth], 0, 480+depth); // args: u_long *clut, x, y

        // unchanged members of primitives are set here
        for(y=0; y<8; y++) {
            for(x=0; x<8; x++)
            {
                // get the current ball sprite pointer
                bp = &db.ball[depth][y][x];

                // initialize the sprite_16 primitive
                SetSprt16(bp);

                // shade texture disable
                SetShadeTex(bp, 1);

                // (u0, v0) = (0, 0)
                setUV0(bp, 0, 0);

                // set texture CLUT ID
                bp->clut = clut;
            }
        }
    }
}

// read controller and move sprites accordingly
int tuto1::pad_read(int *dx, int *dy)
{
    u_long padd; // holds pad data

    // get controller value
    padd = PadRead(1);

    // exit program if select
    if(padd & PADselect) return -1;

    // move sprite position
    if(padd & PADLup)    (*dy)--;
    if(padd & PADLdown)  (*dy)++;
    if(padd & PADLleft)  (*dx)--;
    if(padd & PADLright) (*dx)++;

    return 0;
}

// initialize playstation internals
// not ready to draw after this -
// graphics initialization still required
void tuto1::startSystem(int mode)
{
    RECT rect; // rectangle used to draw on to represent the display

    ResetGraph(0); // initialize renderer
    InitGeom();    // initialize geometry
    DecDCTReset(0); // initialize decompressor
    CdInit();      // initialize CD-ROM - technically not required right here

    SetGraphDebug(0); // set graphics debug mode

    //myPadInit(mode); // initialize the controller
    PadInit(0);

    setRECT(&rect, 0, 0, 256, 240); // set the size of the rectangle, args: x,y,width,height
    ClearImage(&rect, 0, 0, 0); // fills the rectangle with rgb (black in this case - 0,0,0
    SetDispMask(1);

    return;
}

// stop the system; a new program
// can be called with Exec() at this point,
// so that's pretty cool
void tuto1::stopSystem(void)
{
    //myPadStop(); // shut down the controller lib
    PadStop();
    StopCallback(); // StopCallback shuts down most other libraries (minus controller, memory card, etc.)
    return;
}
