// Display_buffer.h

#ifndef DISPLAY_BUFFER_H_INCLUDED
#define DISPLAY_BUFFER_H_INCLUDED

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

typedef struct Display_buffer
{
    DRAWENV draw; // drawing environment
    DISPENV disp; // display environment
    u_long ot[9]; // ordering table
    //SPRT_16 ball[8][8][8]; // sprite buffer,
                           // Z direction: 8,
                           // X direction: 8,
                           // Y direction: 8
} Display_buffer;

#endif // end ifdef DISPLAY_BUFFER_H_INCLUDED


