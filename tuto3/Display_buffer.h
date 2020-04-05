// Display_buffer.h

#ifndef DISPLAY_BUFFER_H_INCLUDED
#define DISPLAY_BUFFER_H_INCLUDED

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

#define OT_LEN 4096 

typedef struct Display_buffer
{
    DRAWENV draw; // drawing environment
    DISPENV disp; // display environment
    u_long ot[OT_LEN]; // ordering table

} Display_buffer;

#endif // end ifdef DISPLAY_BUFFER_H_INCLUDED


