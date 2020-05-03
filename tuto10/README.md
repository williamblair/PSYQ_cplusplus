## Tuto 10: 3D flying over a 2D map

Drawing a 2D map with a 3d perspective, similar to SNES mode 7 pseudo-3d graphics:
https://en.wikipedia.org/wiki/Mode_7

## Idea

Each tile of a map is drawn as a POLY_GT4, but instead of being drawn as a Sprite_textured
in regular 2D, each tile is drawn in 3D with rotation so it appears to be drawn at an
angle horizontally to distance. Additionally, the further back/closer to the top the tile is, 
the darker it is so it appears you have a darkened horizon. Each tile must have a U,V and
positional offset also when moving so it appears the entire map is moving at once.

## Tile_map class

The biggest addition to this sample is the Tile_map class at the beginning
of main.cpp. It contains the array of POLY_GT4 primitives to draw the map tiles
with, as well as the world rotation and transformation vector/matrix. The 
Tile_map constructor first initializes each of the primitives to have a brightness
level from darkest to brightest, top to bottom. Since each primitive is a POLY_GT4,
each corner can have its own color, so in this case we split the brightness of the
current tile in 2 - the top half and the bottom half. 

Next, the coordinates of each
tile are set, relative to the center x,y,z position of the tile map (the center of the screen),
each tile lining up with the next tile so they are seamless.

Finally, the tiles main world translation and angle vectors are initialized. The angle vector
is rotated around the X axis only (by default at least), so tiles at the top appear further
down the Z axis.

Tile::load_texture() loads the single main tile texture once, then copies the texture and CLUT ID's
for each POLY_GT4 primitive.

Finally, Tile::draw() loops over each tile, transforms its coordinates from 3d rotation/translation
to 2d, sets its UV texture offset coordinates based on the map offset, and adds the current tile
to the system order table. The OT depth doesn't matter here as only the tile map set of primitives
are being drawn.

## Tile coordinates to 3d transformed primitive values
The most interesting part of this process (at least compared to previous samples)
is transform_tile_coords(). The current 2D tile coordinates (from the map) are stored in 
a temporary work vector (cur_map_coord), with the Z coordinate set as 0 (since a regular 2D map 
would have no Z coords). The temp vector coordinates are then 3D transformed via a call to 
RotTransPers() for each corner. The results of this calculation are then stored in the 
current POLY primitive to be drawn. The order in the primitives for each corner is in a Z
shape: x0,y0 are top left, x1,y1 are top right, x2,y2 are bottom left, and x3,y3 are bottom
right.

tile_map::update_matrix() works as we've seen before with the cube class, where rotation and
translation are applied to a MATRIX (world_mat), then set to be used via SetRotMatrix() and
SetTransMatrix().

## Tile U,V calculation

For each tile on the map, which tile texture type to be drawn is dependant on our overall
map position. This is calculated by dividing our current offset by the size of a tile, so
we know how many 'tiles' we have traveled. Additionally, to make moving smoother (so we
aren't jumping from tile to tile), the remainder of this division is used to offset the
entire map (if you turn switch the background color to green instead of black, you can
see the entire map shimmy back and forth because of this). This calculation is done
after transform_tile_coords() in Tile_map::draw().


The result should look like this:

![Missing Screenshot](./screenshot.gif "Tuto10 screenshot")

