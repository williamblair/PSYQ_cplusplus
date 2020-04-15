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


// 3D perspective tile map (like pseudo-3d SNES modes)
class Tile_map
{
public:

    Tile_map()
    {
        int i;
        
        // Init primitives
        for (i=0; i<num_tiles; ++i)
        {
            SetPolyG4(&prims[i]);

            // set tile position and color
//            setXYWH(&prims[i], 0, 0, tile_width, tile_height);
            setRGB0(&prims[i], 255, 0, 0);
            setRGB1(&prims[i], 0, 255, 0);
            setRGB2(&prims[i], 255, 0, 255);
            setRGB3(&prims[i], 255, 255, 255);
        }
        
        // Set coordinates
        setVector(&map[0][0], 10, 10, 0);

        // screen x, screen y, screen z
        setVector(&world_position, 0, 240/2, 1024);

        // 360 degrees = 4096
        setVector(&world_angle, -ONE/5, 0, 0);
    }

    void draw()
    {
        long dmy;
        long flg;
        static SVECTOR cur_map_coord;
        static SVECTOR tmp_vector;

        static System * system = System::get_instance();

        // test rotation
        world_angle.vx += 10;

        PushMatrix();

        update_matrix();


        // initial coordinate (top left)
        setVector(&cur_map_coord, map[0][0].vx, map[0][0].vy, 0);

        // Top left
        RotTransPers(&cur_map_coord, (long*)&tmp_vector.vx, &dmy, &flg);
        prims[0].x0 = tmp_vector.vx;
        prims[0].y0 = tmp_vector.vy;
        cur_map_coord.vx += tile_width;

        // Top right
        RotTransPers(&cur_map_coord, (long*)&tmp_vector.vx, &dmy, &flg);
        prims[0].x1 = tmp_vector.vx;
        prims[0].y1 = tmp_vector.vy;
        cur_map_coord.vx -= tile_width;
        cur_map_coord.vy += tile_height;

        // bottom left
        RotTransPers(&cur_map_coord, (long*)&tmp_vector.vx, &dmy, &flg);
        prims[0].x2 = tmp_vector.vx;
        prims[0].y2 = tmp_vector.vy;
        cur_map_coord.vx += tile_width;

        // bottom right
        RotTransPers(&cur_map_coord, (long*)&tmp_vector.vx, &dmy, &flg);
        prims[0].x3 = tmp_vector.vx;
        prims[0].y3 = tmp_vector.vy;

//        printf("Prim coords: (%d,%d),(%d,%d),(%d,%d),(%d,%d)", prims[0].x0, prims[0].y0,
//                                                                prims[0].x1, prims[0].y1,
//                                                                prims[0].x2, prims[0].y2,
//                                                                prims[0].x3, prims[0].y3);
        system->add_prim(&prims[0], 1); // OT depth of 1

        PopMatrix();
    }

private:

    // world transformations
    MATRIX world_mat;
    SVECTOR world_angle;
    VECTOR world_position;

    // The primitives to be transformed and drawn
    static const int num_tiles = 2;
    POLY_G4 prims[num_tiles]; // dummy for now
   
    // tile map positions (top left of each)
    SVECTOR map[2][2]; 

    static const int tile_width = 30;
    static const int tile_height = 30;

    void update_matrix()
    {
        RotMatrix(&world_angle, &world_mat);
        TransMatrix(&world_mat, &world_position);

        SetRotMatrix(&world_mat);
        SetTransMatrix(&world_mat);
    }
};


int main(void)
{
    Tile_map          tile_map;
    System *          system      = System::get_instance();
    Pad               pad1;

    POLY_G4 test_poly;

    // This needs to be called BEFORE system inits
    Pad::init();
    pad1.init_controller(0);

    system->init();
    system->init_graphics();
    system->init_3d();

#if 0
    SetPolyG4(&test_poly);
    setRGB0(&test_poly, 255, 0, 0);
    setRGB1(&test_poly, 255, 255, 0);
    setRGB2(&test_poly, 255, 0, 255);
    setRGB3(&test_poly, 255, 255, 255);
    test_poly.x0 = 10;
    test_poly.y0 = 10;
    test_poly.x1 = 10+100;
    test_poly.y1 = 10;
    test_poly.x2 = 10;
    test_poly.y2 = 10+100;
    test_poly.x3 = 10+100;
    test_poly.y3 = 10+100;
#endif
    
    for (ever)
    {
        system->start_frame();

        pad1.read();

        tile_map.draw();

//        system->add_prim(&test_poly, 1);

        system->end_frame();
    }

    system->deinit();
    return 0;
}

