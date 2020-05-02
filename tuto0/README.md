## Tuto 0: simple sprite drawing

## Initializing the PSX
First, the system class initializes the PSX hardware.

The last three functions in System::init() do the following:
* sets the location (0,0) and size (256,240) of a
  rectangle structure
* Fills the area in VRAM set in the rectangle with the 
  color given by the r,g,b components (0-255)
* Tells the GPU to start displaying by setting the display
  mask to 1

## Initializing PSX graphics
Describing System::init_graphics()

The PSX has an area of VRAM of size 1024x512 px, split
into 64x256 texture pages. This VRAM is used by the GPU
to both store texture data and draw what will be displayed
on screen. This VRAM is inaccessable to the programmer
and is totally handled by the GPU. 

VRAM IMAGE HERE

All drawing and handling of VRAM is done by sending
commands to the GPU.

Back to init_graphics(): First, FntLoad() and SetDumpFnt()
load a basic font texture (I think from within the bios)
into VRAM at x,y location 960,256. Then SetDmpFnt() tells
the SONY library where and what size in VRAM the font
texture is, so then you can draw text on the screen
via the FntPrint() function.

Next, the GPU is told where in VRAM to draw and display
graphics via SetDefDrawEnv() and SetDefDispEnv().
There are two calls each of these so we can have double
buffering. While one area is used for drawing, the other
is displayed. These areas are switched every draw cycle.
In this case, the VRAM area at 0,0 with size 320,240 is used
for drawing while 0,240 with size 320,240 is used for
displaying, all for the first display buffer. The second
display buffer swaps these (0,240 for drawing and 0,0 for
displaying).

IMG HERE

draw[0].isbg and draw[1].isb are flags to the GPU to
say to automatically clear the background each draw
cycle. In this case the background color is set to 0,255,0
(green: r,g,b)

Finally, displaying is enabled with SetDispMak(1) (I think
this may be unnuecessary to be called again..?) 

