// implementation file for tuto0 class
#include "tuto0.hpp"

// main loop of the application
void tuto0::run(void)
{
    // initialize playstation internals
    startSystem(0);

    // run the main loop
    main_tuto();

    // exit library and make inactive
    stopSystem();

    return;
}

// the main loop of the application
void tuto0::main_tuto(void)
{
    // initialize graphics
    DRAWENV draw[2]; // 2 drawing and display environments for double buffer
    DISPENV disp[2];

    POLY_FT4 mat0,mat1; // texture mapped polygon
    SVECTOR x0,x1;      // display position (of the texture?)

    u_short tpage[4]; // texture page id (4 pages)
    u_short clut[4];  // texture clut id
    int     frame = 0; // counter
    int     w = 64, h = 64; // scale
    int     u,v; // work placeholders
    u_long padd; // hold pad data

    // load the bios font to print to the screen
    FntLoad(960, 256); // load the font into position 960, 256 in the frame buffer
    SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512)); // tell where to write to the screen: position 16,16, width 256, height 64, auto background clear (0,1), max text length

    // set 2 drawing and display environments to construct a frame double buffer
    // when buffer 1 is drawing, 2 is being displayed, and vice versa
    // buffer 1: (0, 0)-(320, 240)
    // buffer 2: (0,240)-(320, 480)
    SetDefDrawEnv(&draw[0], 0, 0, 320, 240);
    SetDefDispEnv(&disp[0], 0, 240, 320, 240);
    SetDefDrawEnv(&draw[1], 0, 240, 320, 240);
    SetDefDispEnv(&disp[1], 0, 0, 320, 240);

    // tell the psx to auto clear the bg
    draw[0].isbg = draw[1].isbg = 1;
    setRGB0(&draw[0], 0, 0, 0); // rgb(0,0,0)
    setRGB0(&draw[1], 0,0,0); // same as above

    // initialize polygon primatives for the gs
    SetPolyFT4(&mat0);
    SetShadeTex(&mat0, 1); // ShadeText disable
    SetPolyFT4(&mat1);
    SetShadeTex(&mat1, 1); // ShadeText disable

    // set the initial position of each sprite
    x0.vx = 80-w/2; x1.vx = 240-w/2;
    x0.vy = 120-h/2; x1.vy = 120-h/2;

    // load the texture pattern to the frame buffer,
    // and set the default tpage and CLUT
    load_texture4(tpage, clut);

    // display enable
    SetDispMask(1);

    // initialize the controller library
    //PadInit(0);

    // main loop
    while(1)
    {
        // swap frame double buffer
        PutDrawEnv(frame%2 ? &draw[0] : &draw[1]);
        PutDispEnv(frame%2 ? &disp[0] : &disp[1]);

        // get current controller status
        padd = PadRead(0);

        // change sprite position
        if(padd & PADLup)    x0.vy -= DT;
        if(padd & PADLdown)  x0.vy += DT;
        if(padd & PADLleft)  x0.vx -= DT;
        if(padd & PADLright) x0.vx += DT;

        if(padd & PADRup)    x1.vy -= DT;
        if(padd & PADRdown)  x1.vy += DT;
        if(padd & PADRleft)  x1.vx -= DT;
        if(padd & PADRright) x1.vx += DT;

        // scale
        if(padd & PADL1) w--, h--;
        if(padd & PADR1) w++, h++;

        // end animation
        if(padd & PADselect) break;

        // draw the sprite
        // texture patterns are changed by each
        // frame to do the animation
        mat0.tpage = mat1.tpage = tpage[(frame/16)%4]; // controls front/top/bottom drawing of the sprite (0 and 2 are both front)
        mat0.clut = mat1.clut = clut[(frame/16)%4]; // controls the color table associated with each tpage; indices should match above
        // trying to get them to spin in just 360s
        //mat0.tpage = mat1.tpage = tpage[0];
        //mat0.clut = mat1.clut = clut[0];

        // set texture point (u,v)
        //u = (frame%4)*64;
        //v = ((frame/4)%4)*64;
        u=0;
        v=0;
        setUVWH(&mat0, u, v, 63, 63); // args: p, u, v, width, height
        setUVWH(&mat1, u, v, 63, 63);

        // calculate sprite position
        setXYWH(&mat0, x0.vx, x0.vy, w, h);
        setXYWH(&mat1, x1.vx, x1.vy, w, h);

        // draw the primitives
        DrawPrim(&mat0);
        DrawPrim(&mat1);

        // print debug message
        FntPrint("tuto0: simplest example\n");
        FntPrint("mat0=");dumpXY0(&mat0);
        FntPrint("mat1=");dumpXY0(&mat1);
        FntPrint("Texture Page Index: %d\n", (frame/16)%4);

        // flush the print stream
        FntFlush(-1);

        // wait for V-Blink
        VSync(0);

        // update the frame counter
        frame++;
    }
    // wait for the gpu to catch up
    DrawSync(0);
    return;
}

// initialize playstation internals
// not ready to draw after this -
// graphics initialization still required
void tuto0::startSystem(int mode)
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
void tuto0::stopSystem(void)
{
    //myPadStop(); // shut down the controller lib
    PadStop();
    StopCallback(); // StopCallback shuts down most other libraries (minus controller, memory card, etc.)
    return;
}

void tuto0::load_texture4(u_short tpage[4], u_short clut[4])
{
	/* mat0, mat1, mat2, mat3 are Sprite animation image patterns.
	 * Animation patterns use 4 texture pages, and each page contains
	 * 64 patterns (64x64 size)
	 * Image format here is;
	 *	0x000-0x07f	CLUT  (256x2byte entry)
	 *	0x200-		INDEX (4bit mode, size=256x256) */
    extern	u_long	mat0[];		/* Matching of texture page #0 */
	extern	u_long	mat1[];		/* Matching of texture page #1 */
	extern	u_long	mat2[];		/* Matching of texture page #2 */
	extern	u_long	mat3[];		/* Matching of texture page #3 */

    /* load 4 texture pages.
	 * Since each texture pattern is 4bit mode, it uses 64x256 area
	 * (not 256x256) of the frame buffer.  */
	tpage[0] = LoadTPage(mat0+0x80, 0, 0, 640,  0, 256,256);
	tpage[1] = LoadTPage(mat1+0x80, 0, 0, 640,256, 256,256);
	tpage[2] = LoadTPage(mat2+0x80, 0, 0, 704,  0, 256,256);
	tpage[3] = LoadTPage(mat3+0x80, 0, 0, 704,256, 256,256);

    /* load 4 texture CLUTS */
	clut[0] = LoadClut(mat0, 0,500);
	clut[1] = LoadClut(mat1, 0,501);
	clut[2] = LoadClut(mat2, 0,502);
	clut[3] = LoadClut(mat3, 0,503);
}


