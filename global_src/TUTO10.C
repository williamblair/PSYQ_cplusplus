/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*		   tuto10: 3 dimentional cell type BG
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul,29,1994	suzu 
 *		2.00		May,22,1995	sachiko
 *		2.01		Mar, 9,1997	sachiko	(added autopad)
 */
/*			3D cell-typed-BG */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

/* unit of BG cell is 32x32*/
#define BG_CELLX	32			
#define BG_CELLY	32

/* max width and heght of BG*/
#define BG_WIDTH	1024			
#define BG_HEIGHT	512			
	
/* number of cell*/
#define BG_NX		(BG_WIDTH/BG_CELLX)	
#define BG_NY		(BG_HEIGHT/BG_CELLY)	

/* The depth of OT is4 */
#define OTSIZE		4			

/* screen size is 640x240*/
#define SCR_W		640	
#define SCR_H		240

/* depth of screen*/
#define SCR_Z		512			

/*
 * Define structure to deal BG */
typedef struct {
	SVECTOR		*ang;		/* rotation*/
	VECTOR		*vec;		/* translation*/
	SVECTOR		*ofs;		/* offset on map*/
	POLY_GT4	cell[BG_NY*BG_NX];	/* BG cells*/
} BG;

/*
 * Define structure to deal BG */
typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment*/
	u_long		ot[OTSIZE];	/* OT*/
	BG		bg0;		/* BG 0 */
} DB;

static void init_prim(DB *db,
		      int dr_x, int dr_y, int ds_x, int ds_y, int w, int h);
static void bg_init(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs);
static void bg_update(u_long *ot, BG *bg);
static int pad_read(BG *bg);

void tuto10(void)
{
	/* double buffer*/
	DB		db[2];		
	
	/* current buffer*/
	DB		*cdb;		


	/* initialize GTE*/
	init_system(SCR_W/2, SCR_H/2, SCR_Z);
	
	/* Set initial value of packet buffers.
	 * and make links of primitive list for BG. */
	init_prim(&db[0], 0,     0, 0, SCR_H, SCR_W, SCR_H);
	init_prim(&db[1], 0, SCR_H, 0,     0, SCR_W, SCR_H);

	/* enable to screen*/
	SetDispMask(1);

	/* main loop*/
	cdb = db;
	while (pad_read(&cdb->bg0) == 0) {

		/* exchange primitive buffer*/
		cdb = (cdb==db)? db+1: db;	
		
		/* clear OT*/
		ClearOTag(cdb->ot, OTSIZE);	

		bg_update(cdb->ot, &cdb->bg0);

		DrawSync(0);	/* wait for end of drawing*/
		VSync(0);	/* wait for V-BLNK*/
		
		/* swap double buffer draw*/
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);
		DrawOTag(cdb->ot);	
	}
	DrawSync(0);
	return;
}

/*
 * initialize primitive double buffers 
 * Parameters which would not be changed any more must be set here. */
/* DB	*db,		/* primitive buffer*/
/* int	dr_x, dr_y	/* drawing area location*/
/* int	ds_x, ds_y	/* display area location*/
/* int	w,h		/* drawing/display  area*/
static void init_prim(DB *db,
		      int dr_x, int dr_y, int ds_x, int ds_y, int w, int h)
{
	/* Buffer BG location 
	 * GTE treat angles like follows: 360 degree = 4096 (ONE)  */
	static SVECTOR	ang = {-ONE/5, 0,       0};
	static VECTOR	vec = {0,      SCR_H/2, SCR_Z/2};
	static SVECTOR	ofs = {0,      0,       0};

	/* set double buffer*/
	SetDefDrawEnv(&db->draw, dr_x, dr_y, w, h);
	SetDefDispEnv(&db->disp, ds_x, ds_y, w, h);

	/* set auto clear mode for background */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 0, 0, 0);

	/* initialize for BG*/
	bg_init(&db->bg0, &ang, &vec, &ofs);
}

/***************************************************************************
 * 
 *			BG management */
/*
 * BG cell structure*/
typedef struct {
	u_char	u, v;	/* cell texture UV*/
	u_short	clut;	/* cell texture CLUT*/
	u_long	attr;	/* attribute*/
} CTYPE;

/*
 * BG mesh structure*/
/* 2D array for cell type*/
#include "bgmap.h"	
/* BG texrure pattern/CLUT*/
extern	u_long	bgtex[];

/* initialize BG */
/* BG		*bg,	/* BG data*/
/* int		x,y	/* location on screen*/
/* VECTOR	*vec	/* translation vector*/
/* SVECTOR	*ofs	/* map offset*/

static void bg_init(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs)
{
	POLY_GT4	*cell;
	u_short		tpage, clut;
	int		i, x, y, ix, iy;
	u_char		col;

	/* set location data*/
	bg->ang = ang;
	bg->vec = vec;
	bg->ofs = ofs;

	/* load texture and CLUT*/
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 481);

	/* getnerate primitive list */
	for (cell = bg->cell, iy = 0; iy < BG_NY; iy++) {
		for (ix = 0; ix < BG_NX; ix++, cell++) {

			/* define POLY_GT4*/
			SetPolyGT4(cell);	

			/* change luminace according to Z value */
			/* far side*/
			col = 224*iy/BG_NY+16;		
			setRGB0(cell, col, col, col);
			setRGB1(cell, col, col, col);

			/* near side*/
			col = 224*(iy+1)/BG_NY+16;	
			setRGB2(cell, col, col, col);
			setRGB3(cell, col, col, col);

			/* set tpage*/
			cell->tpage = tpage;	
			
			/* set CLUT*/
			cell->clut  = clut;	
		}
	}
	
}

/*
 * Update BG members */	
/* u_long	*ot,	/* OT*/
/* BG		*bg	/* BG buffer*/
static void bg_update(u_long *ot, BG *bg)
{
	static SVECTOR	Mesh[BG_NY+1][BG_NX+1];

	MATRIX		m;
	POLY_GT4	*cell;
	CTYPE		*ctype;		/* cell type */
	SVECTOR		mp;
	
	/* current absolute position*/
	int		tx, ty;		
	
	/* current left upper corner of map*/
	int		mx, my;		
	
	/* current relative position*/
	int		dx, dy;		
	
	int		ix, iy;		/* work */
	int		xx, yy;		/* work */
	long		dmy, flg;	/* work */

	/* current postion of left upper corner of BG */
	
	/* Lap-round at  BG_CELLX*BG_MAPX , BG_CELLY*BG_MAPY    */
	tx = (bg->ofs->vx)&(BG_CELLX*BG_MAPX-1);
	ty = (bg->ofs->vy)&(BG_CELLY*BG_MAPY-1);

	/* the value obtained by dividing tx by BG_CELLX is the map location (mx)*/
	/* the remainder from the division of tx by BG_CELLX is the displacement (dx)*/
	mx =  tx/BG_CELLX;	my =  ty/BG_CELLY;
	dx = -(tx%BG_CELLX);	dy = -(ty%BG_CELLY);

	PushMatrix();

	/* calculate matrix*/
	RotMatrix(bg->ang, &m);		/* rotation angle*/
	TransMatrix(&m, bg->vec);	/* translation vector*/
	
	/* set matrix*/
	SetRotMatrix(&m);		/* rotation angle*/
	SetTransMatrix(&m);		/* translation vector*/

	mp.vy = -BG_HEIGHT + dy;
	mp.vz = 0;

	/* generate mesh*/
	for (iy = 0; iy < BG_NY+1; iy++, mp.vy += BG_CELLY) {
		mp.vx = -BG_WIDTH/2 + dx; 
		for (ix = 0; ix < BG_NX+1; ix++, mp.vx += BG_CELLX) 
			RotTransPers(&mp, (long *)&Mesh[iy][ix], &dmy, &flg);
	}

	/* Update (u0,v0), (x0,y0) members  */
	for (cell = bg->cell, iy = 0; iy < BG_NY; iy++) {
		for (ix = 0; ix < BG_NX; ix++, cell++) {

			/* check if mesh is in display area or not  */
			if (Mesh[iy  ][ix+1].vx <     0) continue;
			if (Mesh[iy  ][ix  ].vx > SCR_W) continue;
			if (Mesh[iy+1][ix  ].vy <     0) continue;
			if (Mesh[iy  ][ix  ].vy > SCR_H) continue;

			/* wraparound at (BG_MAPX, BG_MAPY)*/
			xx = (mx+ix)&(BG_MAPX-1);
			yy = (my+iy)&(BG_MAPY-1);

			/* get cell type from map database */

			/* notice that Map[][] has a ASCII type ID code. */
			ctype = &CType[(Map[yy])[xx]-'0'];

			/* updatea (u,v),(x,y)*/
			setUVWH(cell, ctype->u, ctype->v,
				BG_CELLX-1, BG_CELLY-1);

			setXY4(cell,
			       Mesh[iy  ][ix  ].vx, Mesh[iy  ][ix  ].vy,
			       Mesh[iy  ][ix+1].vx, Mesh[iy  ][ix+1].vy,
			       Mesh[iy+1][ix  ].vx, Mesh[iy+1][ix  ].vy,
			       Mesh[iy+1][ix+1].vx, Mesh[iy+1][ix+1].vy);

			/* add to OT*/
			AddPrim(ot, cell);
		}
	}
	/* pop matrix*/
	PopMatrix();
}

/*
 * read controller*/
#define DT	8	/* speed */
static int pad_read(BG *bg)
{
	u_long	padd = PadRead(1);
	
	/* quit*/
	if(padd & PADselect) 	return(-1);

	bg->ofs->vy -= 4;

	/* translate*/
	if(padd & PADLup)	bg->ofs->vy -= 2;
	if(padd & PADLdown)	bg->ofs->vy += 2;
	if(padd & PADLleft)	bg->ofs->vx -= 2;
	if(padd & PADLright)	bg->ofs->vx += 2;

	/* rotate*/
	if (padd & PADRup)	bg->ang->vx += DT;
	if (padd & PADRdown)	bg->ang->vx -= DT;
	if (padd & PADRleft) 	bg->ang->vy += DT;
	if (padd & PADRright)	bg->ang->vy -= DT;

	return(0);
}

