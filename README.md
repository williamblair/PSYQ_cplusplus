# PSYQ C++

These are the set of the 13 TUTO graphics samples included with the PSYQ sdk for the
playstation 1, reimplemented in C++ (the original tutorials are written in C).

The original source code for each of these is in the global_src directory

## Compiling

Each directory contains a 'comp.bat' file to compile the example; assumes you
have the PSYQ environment variables set up.

Additionally, the bottom half of each 'comp.bat' contains commands to make a
bin/cue cd image using psx-sdk, using its included mkpsxiso utility. If you
don't have psx-sdk installed (or not located in C:/psx-sdk), you can comment
out those lines by putting a '::' at the beginning of each line (most comp.bat
files already have these commented out)

## Running on real hardware

Most of the examples have been tested on a real PSX using [PSIO](https://ps-io.com).
The only issue I've noticed is that the Pad class (controller) causes the system
to freeze occasionally; from what I've read of this it has to do with VSyncs or something.
Commenting out the pad code from the program fixes this issue as far as I can tell.

## Code structure

Each sample has its main entry point in main.cpp. The remaining files are helper classes.
Not all files are used for each example; investigating main.cpp should indicate what is 
used. The helper classes are as follows:

<table>
<tr><th>Name</th><th>Description</th></tr>
<tr><td>System</td><td>Initializes PSX hardware and manages GPU drawing</td></tr>
<tr><td>Pad</td><td>Manages controllers; buggy currently and causes crashes on real PSX hardware sometimes</td></tr>
<tr><td>Texture</td><td>Sends texture data to the gpu and manages its PSX related info</td></tr>
<tr><td>Sprite,Sprite_textured</td><td>Manages sprite primitives, their locations, textures, and sizes</td></tr>
<tr><td>Cube</td><td>A simple 3D cube object that can be drawn without loading any model data; textured or untextured depending on the sample</td></tr>
</table>