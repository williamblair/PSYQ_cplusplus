## Tuto 12: Screen Warping

Bend the displayed screen image in the shape of 
a sin wave

## Idea

This effect is achieved by drawing the screen
as a vertical stack of sprites (each sprite makes
up a horizontal line of the total screen). The real
screen which we are basing our sprites off of is drawn
elsewhere in VRAM by the GPU, which our sprites use as
the texture area for drawing. After the first DRAWENV (offscreen) area
is drawn, our normal DRAWENV is used to draw each of the screen
line sprites.

## main()

In main(), we define our own DRAWENVs (two for double buffering) (screen_spr_drawenv[2]) 
which we will use along with the internal System class DRAWENVs. We also have an
array of ball sprites, which will just be moved around the screen, nothing too new
or exciting there. Finally we have a single sprite array screen_tex[], which is
the collection of horizontal lines to make up the displayed screen. After the first
main ball textured is loaded, the rest of the ball sprites copy the texture and CLUT
ID's from the first one.

## Manually setting the screen sprite texture area

To initialize the screen sprites, the helper function set_vram_pos() was added
to the Sprite_textured class, which forwards the texture location in VRAM to
read from to the Texture class. Texture::set_vram_pos() calls GetTPage() and GetClut()
on the VRAM texture and clut locations, which returns the calculated texture and clut IDs
from them. Note that the resulting texture page used is the nearest multiple of 64 px wide
and 256 px tall; any texture not aligned to that will need its UV coordinates shifted
by the difference. It's also worth noting that the desginated BPP of the screen textures
is 16 bit since that's the internal storage used by the VRAM.

## Drawing our normal screen vs. the warped screen

Once all of the screen sprites are initialized, the main for(ever) loop begins.
System::start_frame() sets the drawing area used internally, which was modified to
be at VRAM location (320,0), for both display buffers (no "double buffering" in this case).
This area is drawn offscreen, since our display area is still 0,0 and 0,240.
The balls are drawn to this offscreen area using the System class internal order tables.
Note System::end_frame() is called before the main screen sprites are drawn.

The screen is "warped" by moving each screen line sprite left or right by a rsin()
amount, which is PSYQ's fixed point sin function (returning a value between 0 and 4096).
Its input is also between 0 and 4096.

After the ball sprites have been drawn offscreen, we change our DRAWENV to our local 
screen_spr_drawenv area, so we're actually drawing to the display area now. 
Our local screen_spr_ot order table is cleared and drawn to by the sprites via the 
Sprite_textured::add_to_ot() function.


The result should look like this:

![Missing Screenshot](./screenshot.gif "Tuto12 screenshot")

