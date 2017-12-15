/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			tuto9: cell type BG
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul,29,1994	suzu 
 *		2.00		May,22,1995	sachiko
 *		2.01		Mar, 9,1997	sachiko	(added autopad)
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 * 			All rights Reserved
 */
/* Here is a sample program for coventional cell type BG estimation using
 * 8bit/4bit textures.
 * 8bit textured primitive is slower than 4bit textured primite.
 * this sample can draw 4-full-BGs in 1/60 sec using 8bit POLY_FT4 
 *
 * When you want to use special effect like magnify, rotate, and so 
 * on at the BG, you have to use POLY_FT4 instead of SPRT even though
 * POLY_FT4  is 2 times slower than SPRT.
 *
 * POLY_FT4 primitives are neccesary for magnified or rotated BG cells, 
 * but it is slower than SPRT. therefore if you do not need these functions,
 * you should use SPRT instead of POLY_FT4. Please define BG_SPRT when you
 * want to use SPRT primitevs as each BG cells.
 * */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

/* unit of BG cell is 32x32*/
#define BG_CELLX	32
#define BG_CELLY	32

#define BG_WIDTH	288	/* max BG width*/
#define BG_HEIGHT	192	/* max BG height*/

/* number of BG cells*/
#define BG_NX		(BG_WIDTH/BG_CELLX+2)
#define BG_NY		(BG_HEIGHT/BG_CELLY+2)

/* depth of OT*/
#define OTSIZE		4	

/* Define BG structure*/
typedef struct {
	/* local DRAWENV for each BG*/
	DRAWENV		env;	
	
	/* local OT for each BG*/
	u_long		ot[OTSIZE];	
	
	SPRT		cell[BG_NY*BG_NX];	/* BG cells (SPRT) */
	POLY_FT4	cell2[BG_NY*BG_NX];	/* BG cells (POLY_FT4) */
} BG;

/*
 * primitive double buffer*/
#define MAXBG	4			/* max 4 BGs*/
typedef struct {
	DISPENV	disp;			/* display environment*/
	BG	bg[MAXBG];		/* BG */
} DB;

/* define this when you use SPRT BG */
#define BG_SPRT

/* define this when you use 4bit texture */
#define BG_4bit

static void bg_init(BG *bg, int x, int y);
static void bg_update(BG *bg, SVECTOR *mapxy);
static void bg_draw(BG *bg);
static int pad_read(SVECTOR *mapxy);

void tuto9(void)
{
	/* primitive double buffer*/
	DB	db[2];		
	
	/* current double buffer*/
	DB	*cdb;		
	
	/* current OT*/
	u_long	*ot;		
	
	/* current location of BG*/
	SVECTOR	mapxy[4];	
	
	int	i;

	/* clear frame buffer*/
	{
		RECT rect;
		setRECT(&rect, 0, 0, 320, 480);
		ClearImage(&rect, 0, 0, 0);
	}

	/* define frame double buffer*/
	/*	Buffer0   #0:	(  0   0) -(320,240)
	 *	Buffer1   #1:	(  0,240) -(320,480)
	 */
	SetDefDispEnv(&db[0].disp,  0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp,  0,   0, 320, 240);

	/* Set Initial value of primitive buffer
	 * link primitive list for BG here. */

	bg_init(&db[0].bg[0], 0, 4);		/* BG #0 */
	bg_init(&db[1].bg[0], 0, 4+240);

	bg_init(&db[0].bg[1], 20, 12);		/* BG #1 */
	bg_init(&db[1].bg[1], 20, 12+240);

	bg_init(&db[0].bg[2], 40, 20);		/* BG #2 */
	bg_init(&db[1].bg[2], 40, 20+240);

	bg_init(&db[0].bg[3], 60, 28);		/* BG #3 */
	bg_init(&db[1].bg[3], 60, 28+240);

	/* Set map initial value*/
	setVector(&mapxy[0],  0,  0,0);	/* default position (  0,  0) */
	setVector(&mapxy[1],256,256,0);	/* default position (256,256) */
	setVector(&mapxy[2],  0,256,0);	/* default position (  0,256) */
	setVector(&mapxy[3],256,  0,0);	/* default position (256,  0) */

	/* display enable*/
	SetDispMask(1);

	/* main loop*/
	while (pad_read(mapxy) == 0) {		/* acquire mapxy*/

		/* swapt primitive buffer ID */
		cdb = (cdb==db)? db+1: db;

		/* update BG*/
		for (i = 0; i < 4; i++)
			bg_update(&cdb->bg[i], &mapxy[i]);

		/* swap double buffer*/
		DrawSync(0);
		VSync(0);
		PutDispEnv(&cdb->disp); 

		/* draw 4 plane of BG*/
		for (i = 0; i < 4; i++)
			bg_draw(&cdb->bg[i]);
	}
	DrawSync(0);
	return;
}

/***************************************************************************
 *
 *	BG management */
/* BG cell structure
 * This sample does not use clut member because it uses only 1 clut.
 * This sample does not use attr member because no attributes are defined here.
 * See bgmap.h for detail. */
typedef struct {
	u_char	u, v;	/* cell texture UV*/
	u_short	clut;	/* cell texture CLUT*/
	u_long	attr;	/* attribute*/
} CTYPE;

/* 2D array for cell type*/
#include "bgmap.h"		

#ifdef BG_4bit
extern	u_long	bgtex[];	/* BG texture\CLUT (4bit) */
#else
extern	u_long	bgtex8[];	/* BG texture\CLUT (8bit) */
#endif

/*
 * initialize BG.
 * All parameters which would not be changed is set here. */
/* BG	*bg,	/* BG data */
/* int	x,y	/* location on screen */
static void bg_init(BG *bg, int x, int y)
{
	SPRT		*cell;
	POLY_FT4	*cell2;
	u_short		clut, tpage;
	int		ix, iy;

	/* set drawing environment*/
	SetDefDrawEnv(&bg->env, x, y, BG_WIDTH, BG_HEIGHT);

	/* load texture data and CLUT*/

#ifdef BG_4bit	/* 4bit mode */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 480);
#else		/* 8bit mode */
	tpage = LoadTPage(bgtex8+0x80, 1, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex8, 0, 480);
#endif
	/* set default texture page*/
	bg->env.tpage = tpage;

	/* clear local OT*/
	ClearOTag(bg->ot, OTSIZE);

	/* make primitive list for BG */

	for (cell = bg->cell, cell2 = bg->cell2, iy = 0; iy < BG_NY; iy++)
	    for (ix = 0; ix < BG_NX; ix++, cell++, cell2++) {

#ifdef BG_SPRT
		SetSprt(cell);			/* SPRT Primitive */
		SetShadeTex(cell, 1);		/* ShadeTex forbidden */
		setWH(cell, BG_CELLX, BG_CELLY);/* Set the sizes of the cell */
		cell->clut = clut;		/* Set texture CLUT  */
		AddPrim(&bg->ot[0], cell);	/* Put SPRT primitive into OT */
#else
		SetPolyFT4(cell2);		/* POLY_FT4 Primitive */
		SetShadeTex(cell2, 1);		/* ShadeTex forbidden */
		cell2->tpage = tpage;		/* Set texture pages*/
		cell2->clut  = clut;		/* Set texture CLUT*/
		AddPrim(&bg->ot[0], cell2);	/* Put POLY_FT4 primitives 
						   into OT  */
#endif
	    }
}

/* updatea BG members*/
/* BG		*bg,	/* BG data */
/* SVECTOR	*mapxy	/* map location */

static void bg_update(BG *bg, SVECTOR *mapxy)
{
	/* cell data for sprites*/
	SPRT		*cell;		
	
	/* cell data for polygon*/
	POLY_FT4	*cell2;		
	
	/* cell type*/
	CTYPE		*ctype;		
	
	/* absolute postion on map*/
	int		mx, my;		
	
	/* relative position on map*/
	int		dx, dy;		
	
	int		tx, ty;		/* work */
	int		x, y, ix, iy;

	/* (tx, ty) is rap-rounded at BG_CELLX*BG_MAPX  */
	tx = (mapxy->vx)&(BG_CELLX*BG_MAPX-1);
	ty = (mapxy->vy)&(BG_CELLY*BG_MAPY-1);

	/*  the value obtained by dividing tx by BG_CELLX is the map location (mx)*/
	/*  the remainder from the division of tx by BG_CELLX is the displacement (dx)*/
	mx =  tx/BG_CELLX;	my =  ty/BG_CELLY;
	dx = -(tx%BG_CELLX);	dy = -(ty%BG_CELLY);

	/* update (u0,v0), (x0,y0) in the primitive list. */

	cell  = bg->cell;
	cell2 = bg->cell2;
	for (iy = y = 0; iy < BG_NY; iy++, y += BG_CELLY) {
		for (ix = x = 0; ix < BG_NX; ix++, x += BG_CELLX) {

			/* rap-rounded at BG_MAPX, BG_MAPY  */
			tx = (mx+ix)&(BG_MAPX-1);
			ty = (my+iy)&(BG_MAPY-1);

			/* get cell-type from the map data.
			 * Id code is stored  as a ASCII code in Map[][]  */ 
			ctype = &CType[(Map[ty])[tx]-'0'];
#ifdef BG_SPRT
			/* to reduce the memory access, update only
			 * (u0,v0), (x0, y0) menbers */
			setUV0(cell, ctype->u, ctype->v);
			setXY0(cell, x+dx, y+dy);
#else
			/* to reduce the memory access, update only
			 *  (u0,v0), (x0, y0) menbers (POLY_FT4) */
			setUVWH(cell2, ctype->u, ctype->v,
				BG_CELLX-1, BG_CELLY-1);
			setXYWH(cell2, x+dx, y+dy, BG_CELLX, BG_CELLY);
#endif
			cell++;
			cell2++;
		}
	}
}

/*
 * draw BG*/
static void bg_draw(BG *bg)
{
	/* Update Drawing environment for each BG */
	PutDrawEnv(&bg->env);	
	
	DrawOTag(bg->ot);	/* draw*/
}

/*
 * read controller*/
/* SVECTOR *mapxy	/* map location */
static int pad_read(SVECTOR	*mapxy)
{
	static SVECTOR	v[4] = {
		 0, 0, 0, 0,	0,  0, 0, 0,
		 0, 0, 0, 0,	0,  0, 0, 0,
	};
	
	int	i,id = 3;
	u_long	padd = PadRead(1);

	/* quit*/
	if(padd & PADselect) 	return(-1);

	/* select window ID*/
	if (padd&PADR1) id = 3;
	if (padd&PADR2) id = 2;
	if (padd&PADL1) id = 1;
	if (padd&PADL2) id = 0;

	/* move BG*/
	if(padd & PADLup)	setVector(&v[id],  0, -2, 0);
	if(padd & PADLdown)	setVector(&v[id],  0,  2, 0);
	if(padd & PADLleft)	setVector(&v[id], -2,  0, 0);
	if(padd & PADLright)	setVector(&v[id],  2,  0, 0);

	/* Add the motion range */
	for (i = 0; i < 4; i++)
		addVector(&mapxy[i], &v[i]);

	return(0);
}


