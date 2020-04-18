// blank main template for psyq c++

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

#include "System.h"
#include "Sprite.h"
#include "Cube.h"
#include "Pad.h"


#define ever ;;

// Texture and CLUT
extern u_long bgtex[];

#define BG_MAPX		32		/* map size (X) */
#define BG_MAPY		32		/* map size (Y) */
#define BG_TEXX		32
#define BG_TEXY		32

/*
 * BG cell structure*/
typedef struct {
	u_char	u, v;	/* cell texture UV*/
	u_short	clut;	/* cell texture CLUT*/
	u_long	attr;	/* attribute*/
} CTYPE;

static CTYPE CType[] = {	
	0*BG_TEXX, 3*BG_TEXY, 0, 0,		/* 0: marsh */
	2*BG_TEXX, 3*BG_TEXY, 0, 0,		/* 1: board */
	3*BG_TEXX, 3*BG_TEXY, 0, 0,		/* 2: grass */
	0*BG_TEXX, 4*BG_TEXY, 0, 0,		/* 3: bush */
	0*BG_TEXX, 6*BG_TEXY, 0, 0,		/* 4: broad-leaved tree */
	2*BG_TEXX, 6*BG_TEXY, 0, 0,		/* 5: coniferous tree */
	2*BG_TEXX, 5*BG_TEXY, 0, 0,		/* 6: sand */
	3*BG_TEXX, 5*BG_TEXY, 0, 0,		/* 7: rock */
	0*BG_TEXX, 5*BG_TEXY, 0, 0,		/* 8: low mountain */
	0*BG_TEXX, 7*BG_TEXY, 0, 0,		/* 9: high mountain */
};

static char *Map[] = {
	"88232323234455542323277667899998",
	"88822322232445554223227766789998",
	"22223222324445542232277667899998",
	"22322122322345542327676678998232",
	"23322322232455442313277667898233",
	"22332232223245544223227766728854",
	"43322322345555544223227766728888",
	"45431323455554422322776676724884",
	"23223223455554422322776672854222",
	"22322345555442232277667672232283",
	"32232455442232277667672223223883",
	"32123444233267676722123242388543",
	"32232423327676767223223442388543",
	"22223242332766676722322423883332",
	"22223242332767672232442232324232",
	"88223222324555542232277667899998",
	"88223222324455542232277667899998",
	"82222322232445554223227766789998",
	"22223222324445542232277667899998",
	"22322122322345542327676678998232",
	"23322322232455442313277667898233",
	"22332232223245544223227766728854",
	"43322322345555544223227766728888",
	"45431323455554422322776676724484",
	"23223223455554422322776672852452",
	"22322345555442232277667672232483",
	"32232455442236277667672223223883",
	"32123444233267676722124422388543",
	"32232423324266767223267662388543",
	"22223242332425276722326623883332",
	"82223242332455242232442232324232",
	"88223222324555542232277667899998",
};

// 3D perspective tile map (like pseudo-3d SNES modes)
class Tile_map
{
public:

    Tile_map()
    {
        int i;
        int row;
        int col;
        int brightness;
        int tile_x;
        int tile_y;
        int tile_offset_x = 320/2; // offset so the map is centered on the screen
        int tile_offset_y = 240/2;

        player_x = player_y = 0;
        
        // Init primitives
        i = 0;
        brightness = 0;
        for (row = 0; row < 32; ++row)
        {
            for (col = 0; col < 32; ++col)
            {
        
            SetPolyGT4(&prims[i]);

            // Set tile brightness
            // further towards the top is 'further in the distance'
            // so it will be darker like its far away
            setRGB0(&prims[i], brightness, brightness, brightness);
            setRGB1(&prims[i], brightness, brightness, brightness);
            setRGB2(&prims[i], brightness+4, brightness+4, brightness+4);
            setRGB3(&prims[i], brightness+4, brightness+4, brightness+4);

            ++i;
            }
            
            brightness += 8;
            if (brightness > 255) brightness = 255;
        }
        
        // Set coordinates
        // each coordinate represents the top left
        for (row = 0; row < 32; ++row)
        {
            for (col = 0; col < 32; ++col)
            {
                tile_x = (col * tile_width)  - tile_offset_x;
                tile_y = (row * tile_height) - tile_offset_y;

                setVector(&map[row][col], tile_x, tile_y, 0);
            }
        }

        // screen x, screen y, screen z
        // geom and screen offset is center of the screen + 512 depth,
        // this is relative to that
        setVector(&world_position, 0, 0, 0);

        // 360 degrees = 4096
        setVector(&world_angle, -ONE/5, 0, 0);
    }

    void load_texture()
    {
        int i;
        
        map_texture.load(
            bgtex+0x80,     // texture data
            bgtex,          // clut data
            TEXTURE_4BIT,   // bpp
            640, 0,         // VRAM tex location
            256, 256,       // texture size
            0, 481          // VRAM clut location
        );
        
        for (i = 0; i < num_tiles; ++i)
        {
            // defined above
            //setUVWH(&prims[i], CType[1].u, CType[1].v, BG_MAPX-1, BG_MAPY-1);
            prims[i].tpage = map_texture.get_texture_page_id();
            prims[i].clut = map_texture.get_clut_id();
        }
    }

    void draw()
    {
        int  row;
        int  col;
        int  prim_index = 0;
        int  ctype_index = 0;
        int  tile_index_offset_x;
        int  tile_index_offset_y;
        int  tile_index_x;
        int  tile_index_y;
        //long dmy;
        //long flg;
        //static SVECTOR cur_map_coord;
        //static SVECTOR tmp_vector;

        static System * system = System::get_instance();

        // test rotation
        //world_angle.vx += 10;

        PushMatrix();

        // update world rotation/translation
        update_matrix();

        // Calculate the current top left map index 
        // (which tile x and y in *Map[] is our starting pos?)
        tile_index_offset_x = player_x/tile_width;
        tile_index_offset_y = player_y/tile_height;

        for (row = 0; row < 32; ++row)
        {
            for (col = 0; col < 32; ++col)
            {

            // rotate and skew the tile coordinates based on world rotation/translation
            transform_tile_coords(row, col, prim_index);


            // UV coordinates
            tile_index_x = (col + tile_index_offset_x) & 31; // wrap around once reach the number of tiles
            tile_index_y = (row + tile_index_offset_y) & 31;
            ctype_index = (int)(Map[tile_index_y][tile_index_x] - '0');
            setUVWH(&prims[prim_index], CType[ctype_index].u, CType[ctype_index].v, tex_width-1, tex_height-1);

//        printf("Prim coords: (%d,%d),(%d,%d),(%d,%d),(%d,%d)", prims[prim_index].x0, prims[prim_index].y0,
//                                                                prims[prim_index].x1, prims[prim_index].y1,
//                                                                prims[prim_index].x2, prims[prim_index].y2,
//                                                                prims[prim_index].x3, prims[prim_index].y3);
            system->add_prim(&prims[prim_index], 1); // OT depth of 1

            ++prim_index;           
 
            }
        }
        
        PopMatrix();
    }

    void move_player(const int x, const int y)
    {
        player_x += x;
        player_y += y;

        // wrap around if player moved past the edge of the map
        if (player_x < 0) player_x = 32*tile_width + player_x;
        if (player_x >= 32*tile_width) player_x -= 32*tile_width;

        if (player_y < 0) player_y = 32*tile_height + player_y;
        if (player_y >= 32*tile_height) player_y -= 32*tile_height;
    }

private:

    // world transformations
    MATRIX world_mat;
    SVECTOR world_angle;
    VECTOR world_position;

    // map offset / 'player' coordinates
    int player_x;
    int player_y;

    // The primitives to be transformed and drawn
    static const int num_tiles = 32*32;
    POLY_GT4 prims[num_tiles]; // dummy for now
   
    // tile map positions (top left of each)
    SVECTOR map[32][32]; 

    static const int tile_width = 15;
    static const int tile_height = 15;

    static const int tex_width = 32;
    static const int tex_height = 32;

    Texture map_texture;

    void update_matrix()
    {
        RotMatrix(&world_angle, &world_mat);
        TransMatrix(&world_mat, &world_position);

        SetRotMatrix(&world_mat);
        SetTransMatrix(&world_mat);
    }

    inline void transform_tile_coords(const int row, const int col, int prim_index)
    {
        long dmy;
        long flg;
        SVECTOR cur_map_coord;
        SVECTOR tmp_vector;
        
        // initial coordinate (top left)
        setVector(&cur_map_coord, map[row][col].vx, map[row][col].vy, 0);

        // Top left
        RotTransPers(&cur_map_coord, (long*)&tmp_vector.vx, &dmy, &flg);
        prims[prim_index].x0 = tmp_vector.vx;
        prims[prim_index].y0 = tmp_vector.vy;
        cur_map_coord.vx += tile_width;

        // Top right
        RotTransPers(&cur_map_coord, (long*)&tmp_vector.vx, &dmy, &flg);
        prims[prim_index].x1 = tmp_vector.vx;
        prims[prim_index].y1 = tmp_vector.vy;
        cur_map_coord.vx -= tile_width;
        cur_map_coord.vy += tile_height;

        // bottom left
        RotTransPers(&cur_map_coord, (long*)&tmp_vector.vx, &dmy, &flg);
        prims[prim_index].x2 = tmp_vector.vx;
        prims[prim_index].y2 = tmp_vector.vy;
        cur_map_coord.vx += tile_width;

        // bottom right
        RotTransPers(&cur_map_coord, (long*)&tmp_vector.vx, &dmy, &flg);
        prims[prim_index].x3 = tmp_vector.vx;
        prims[prim_index].y3 = tmp_vector.vy;
    }
};


int main(void)
{
    Tile_map          tile_map;
    System *          system      = System::get_instance();
    Pad               pad1;

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();
    system->init_3d();

    tile_map.load_texture();

    for (ever)
    {
        system->start_frame();

        pad1.read();
        if (pad1.is_held(PadLeft))  tile_map.move_player(-5, 0);
        if (pad1.is_held(PadRight)) tile_map.move_player( 5, 0);
        if (pad1.is_held(PadUp))    tile_map.move_player(0, -5);
        if (pad1.is_held(PadDown))  tile_map.move_player(0, 5);

        tile_map.draw();


        system->end_frame();
    }

    system->deinit();
    return 0;
}

