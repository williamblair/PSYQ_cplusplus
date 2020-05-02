## Tuto 9: Different Viewports

Shows how to use different viewports when drawing for multi-player
splitscreen type situations or similar

## Idea

This sample achieves drawing in multiple local windows by
setting multiple DRAWENVs instead of a single pair of 2 done in
all previous tutorials. The DISPENVs remain the same since you're
always displaying the same total screen regardless of which area
you're drawing in. All drawing is done relative to the current
DRAWENV, so each square (as seen in the screenshot) has the same
internal primitive coordinates, just each are drawn relative to
the top left of the current DRAWENV.

## System class changes

Internally, the System class now has 2 display buffers for each viewport
(top left, top right, bottom left, and bottom right of the screen). Note that this
means each viewport has its own order tables (2) as well.
The start_frame(), end_frame(), and add_prim() functions have been modified
to take in which viewport is currently being referred to as the last
argument. 

The System::init_graphics() function now initializes DRAWENV and DISPENVs for
each viewport display buffer, with the background being set to autoclear as a
different color for each.

System::end_frame() has removed the FntFlush(), DrawSync(), and VSync() functions
as we only want these called once all the viewports have been drawn, otherwise
you'd effectively be running at 1/4 the normal speed. These are instead called
in the main loop for this sample.

An additional function, set_dispenv() has been added so you don't have to make
redundant calls to PutDispEnv() since each DISPENV is the same for each viewport.

## main()
The main function as an array of four POLY_F4s (testpoly) that we will
draw one of each each viewport. 

Inside the main loop, first the main DISPENV for the current display buffer is
set via Syste::set_dispenv(). Next, each viewport is looped over, with the
usual start_frame(), add_prim(), and end_frame(), besides the new which
viewport argument (i).

DrawSync() is called inside each viewport so the FntPrint() calls draw the text
in the correct area for the current viewport. VSync() is called after all viewport OTs
have been sent to the GPU.

The result should look like this:

![Missing Screenshot](./screenshot.gif "Tuto8 screenshot")

