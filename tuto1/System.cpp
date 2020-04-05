// System.cpp

#include "System.h"

// static instance variable
System System::instance_obj;

void System::init()
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
}

void System::init_graphics()
{
    // load the bios font to print to the screen
    FntLoad(960, 256); // load the font into position 960, 256 in the frame buffer
    SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512)); // tell where to write to the screen: position 16,16, width 256, height 64, auto background clear (0,1), max text length

    // set 2 drawing and display environments to construct a frame double buffer
    // when buffer 1 is drawing, 2 is being displayed, and vice versa
    // buffer 1: (0, 0)-(320, 240)
    // buffer 2: (0,240)-(320, 480)
    SetDefDrawEnv(&disp_buffs[0].draw, 0, 0, 320, 240);
    SetDefDispEnv(&disp_buffs[0].disp, 0, 240, 320, 240);
    SetDefDrawEnv(&disp_buffs[1].draw, 0, 240, 320, 240);
    SetDefDispEnv(&disp_buffs[1].disp, 0, 0, 320, 240);

    // tell the psx to auto clear the bg
    disp_buffs[0].draw.isbg = disp_buffs[1].draw.isbg = 1;
    setRGB0(&disp_buffs[0].draw, 0,255,0); // rgb(0,0,0)
    setRGB0(&disp_buffs[1].draw, 0,255,0); // same as above

    // disp_buffs[0].display enable
    SetDispMask(1);
}

void System::start_frame()
{
    // swap frame double buffer
    PutDrawEnv(cur_buf ? &disp_buffs[1].draw : &disp_buffs[0].draw);
    PutDispEnv(cur_buf ? &disp_buffs[1].disp : &disp_buffs[0].disp);
    cur_buf = !cur_buf;
}

void System::end_frame()
{
    FntFlush(-1);
    VSync(0);
}

void System::deinit()
{
    //myPadStop(); // shut down the controller lib
    PadStop();
    StopCallback(); // StopCallback shuts down most other libraries (minus controller, memory card, etc.)
    return;
}

