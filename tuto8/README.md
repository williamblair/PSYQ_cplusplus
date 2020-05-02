## Tuto 8: Scrolling backgrounds

This sample moves totally away from 3d and instead shows a scrolling
background technique

## Idea

The main idea behind this sample is that you will combine
the left and right side of a seamless texture in two different sprites to achieve
an automatic wraparound effect by offsetting both sprites relative
to an 'offset' x value for moving left and right. The left and right sides
will swap every other wraparound for continuity.

## main()

Looking at main(), there are 5 different Sprite_textured instances. There are
two for each background (a near and far background), as well as a single
window sprite to look like you're riding on a train

The variables offset_x, left_right, and velocity are for the following:
* offset_x: the variable which controls the 'movement' left and right; it
  is the point where the left and right side of the textures meet up next to
  each other
* left_right: if non-zero, draw the second sprite on the left and first sprite on the right;
  else draw them vice-versa
* velocity: how fast to move offset_x

## System::init_graphics()

In this sample, our DISPENV and DRAWENV environments have been changed to have a width
of 256 instead of 320. This is because the max width and height of a texture that can be drawn on 
a primitive is 256px (although in practice, 255 only seems to work), or 4 texture pages wide.
So, limiting our screen size makes this easier and prevents an awkward gap on the left and
right side of the screen.

## Sprite_textured

After System::init() and System::init_graphics(), each sprite is loaded. This time,
the textures are 8bit instead of 4bit.

Additionally, the internal primitives used to draw the texture has been changed from
POLY_FT4 to a combination of a DR_MODE (draw mode) and SPRT. According to the original
sample code, this combination is faster than using POLY\* primitives. These two primitives
have to be 'merged' together via MargePrim() (no, that's not a typo! did the PSYQ devs not
spell merge properly...?). The SPRT portion of the primitive basically replaces and acts the
same as POLY_FT4 did previously, while the only major difference (besides MargePrim()) is
that the DR_MODE primitive needs to be initialized via SetDrawMode(), in which you send
the texture ID. The clut ID still goes in the sprite prim like normal 
(see Sprite_textured::load_texture()). Another important note is that the two DR_MODE and SPRT
need to be stored together in a struct, with DR_MODE coming FIRST (order matters!).

## Back to main()

In the main loop, each of the combos of sprites are drawn via draw_far_bg() and 
draw_hills_bg(), which both function basically the same. The only difference is that
the offset position is scaled differently in both functions (divided by 2 and 4) so
the further back background moves slower than the closer background.

In draw_far_bg(), the first thing done is figure out which sprite part is drawn on the
left or right side, based on the offset_x value. Next, the u,v top left texture offset
is set for the left and right sprite. The left sprite always has a u (x) texture offset
of whatever the current horizontal distance is, then the right sprite always has a 
texture u offset of 0, so it lines up exactly where the left sprite leaves off.

The X screen position of the right sprite is equal to distance so it lines up with
the right side of the left sprite as well.

Finally, both the left and right sprite are added to the system order table at the same
distance (either 3, furthest back, or 2 depending on draw_far_bg() and draw_hills_bg()).

The result should look like this:

![Missing Screenshot](./screenshot.gif "Tuto8 screenshot")

