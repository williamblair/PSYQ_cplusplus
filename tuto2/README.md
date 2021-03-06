## Tuto 2: Basic 3D Plane

In this sample we start drawing in 3D with a simple plane surface.
It's also gouraud shaded!

## 3D on the PSX

3D Drawing on the PS1 is done by converting 3d coordinates to 2d based
on how the scene would look from the camera's perspective. The math
required for this is done on a special coprocessor in the PS1's CPU
called the GTE - geometry transformation engine. The GTE is responsible
for vector and matrix math for 3d calculations. These calculations produce
x and y coordinates which are then stored in primitives and drawn the same
as seen in tuto0 and tuto1.

3d transformation calculations are done by storing matrices in the GTE registers,
via functions like SetRotMatrix() and SetTransMatrix(),
which the GTE will use when doing matrix transformations.

## Initializing 3D on the system

Tracing main(), besides System::init() and System::init_graphics(), we additionally
have System::init_3d() now. init_3d() calls 2 library functions: SetGeomOffset() and
SetGeomScreen(). SetGeomOffset() sets the center 2d point from which 3d coordinate calculations
are relative to. Here we set them to 160,120 - the center of the screen. SetGeomScreen() sets
the Z distance from the camera eye to the drawing point.

## main: Square
After init_3d() is called in main, init_square() is called. This initializes 
a POLY_G4 primitive in basically the same way as Sprite_textured does. We do not
used the Sprite_textured class here as the square has no texture. Additionally,
the primitive is gourouad shaded (the G in POLY_G4), so we can give it four
different colors which will be blended when drawn (setRGB0 through setRGB3, as 
opposed to a single setRGB0 in the previous samples).

## main: Screen printing
FntPrint() has been mentioned previously, which works the same as the printf() C
functions, except the text is drawn on screen as opposed to printing out to the console.
Additionally, dumpXY4() and dumpVector() are provided by the PSYQ lib as ways to more
easily call FntPrint() with vector information

## 3D math calculations
After the debug screen drawings, System::start_frame() is called which works the
same way as the previous sample, clearing the order table. Next, the function
calculate_world() fills in the MATRIX structure M with rotation and translation
information, based on the angle vector and world_coords vector. Matrix M is then
stored in the GTE via SetRotMatrix() and SetTransMatrix(). These internal GTE
values will then be used when translating 3d primitive coordinates to 2d.

Looking at the values in angle and world_coords, we see the values 512,512,512, which
are clearly larger than 360 degrees and are also not radians. The GTE uses values
relative to 4096, which is defined as ONE in the PSYQ libraries. This is due to
the fact that the PSX uses fixed point math 
(https://en.wikipedia.org/wiki/Q_%28number_format%29), where each number represents
its floating point version multiplied by 4096 (2^12). Hence, 4096 = 1.0, since 
4096/4096 = 1. So, the angle 512 corresponds to 2\*PI/8 radians = 45 degrees, since
512/4096 = 1/8.

After the rotation and translation matrices have been set in the GTE, we then call
transform_square(). This transforms the square primitives 2d coordinates to look
like 3d based on our camera perspective. The starting square coordinates are
variants of +-128. These values are relative to the center of our screen which was
set via SetGeomOffset() earlier, i.e. the real drawn screen X coordinate of -128
is -128 + 160 = 32.

The 3d to 2d calculation is done via calls to RotTransPers, which takes a coordinates
vector (an x,y,z value) from square_coords, and stores the transformed result
in the x,y values pointer to by the second arg, in this case our square
POLY_G4 primitive values. This is done 4 times, one for each square coordinate
(top left, top right, bottom left, bottom right).

Finally, the transformed values in square POLY_G4 are drawn via addition to the system's
current order table (system->add_prim) at a depth of 0. The depth here really
doesn't matter as no other primitives are being drawn so order doesn't matter.
System::end_frame() works the same as previously and sends the order table list
to the GPU via DrawOTag().

When we run the program we should see the following:

![Missing Screenshot](./screenshot.png "Tuto2 screenshot")

