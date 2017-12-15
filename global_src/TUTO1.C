/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*				tuto1: OT
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/*			  simple sample to use OT */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

/*
 * Primitive double buffer.
 * Primitive buffer should be 2 buffers for CPU and GPU. */	
typedef struct {		
	DRAWENV		draw;		/* Drawing Environment */
	DISPENV		disp;		/* Display Environment	*/
	u_long		ot[9];		/* Ordering Table */
	SPRT_16		ball[8][8][8];	/* sprite buffer */
					/* Z-direction	: 8*/
					/* X-direction	: 8*/
					/* Y-direction	: 8*/
} DB;

static void init_prim(DB *db);
static int pad_read(int *dx, int *dy);

void tuto1(void)
{
	DB	db[2];		/* primitive double buffer */
	DB	*cdb;		/* current primitive buffer pointer */
	SPRT_16	*bp;		/* work */
	int	ox, oy;		/* work */
	int	dx = 0, dy = 0;	/* work */
	int	depth;		/* work */
	int	x, y;		/* work */
	int	ix, iy;		/* work */

	/* reset graphic subsystem*/
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));
	
	/* define frame double buffer */
	/*	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,240)-(320,480) (320x240)
	 */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* set primitive members of each buffer */
	init_prim(&db[0]);	/* packet buffer #0*/
	init_prim(&db[1]);	/* packet buffer #1*/

	/* display enable */
	SetDispMask(1);

	/* main loop */
	while (pad_read(&dx, &dy) == 0) {

		/* swap db[0], db[1]*/
		cdb = (cdb==db)? db+1: db;

		/* clear ordering table (8 entry) */
		ClearOTag(cdb->ot,8);

		/* calculate postion of sprites */
		for (depth = 0; depth < 8; depth++) {
			/* Far sprites move slowly, and near sprites
			 * move fast. */
			/* left upper corner (Y)*/
			oy =  56 + dy*(depth+1);	
			
			/* left upeer corner (X)*/
			ox =  96 + dx*(depth+1);	
 			for (iy = 0, y = oy; iy < 8; iy++, y += 16) 
			for (ix = 0, x = ox; ix < 8; ix++, x += 16) {

				bp = &cdb->ball[depth][iy][ix];

				/* set upper-left corner of sprites */
				setXY0(bp, x, y);

				/* add primitives to OT */
				AddPrim(&cdb->ot[depth], bp);
			}
		}
		/* wait for previous 'DrawOTag'  */
		DrawSync(0);

		/* wait for next V-BLNK */
		VSync(0);

		/* swap frame double buffer
		 *  set the drawing environment and display environment. */
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);

		/* start Drawing */
		DrawOTag(cdb->ot);
		FntPrint("tuto1: OT\n");
		FntFlush(-1);
	}
	DrawSync(0);
	return;
}

/* Intitalize the members of primitives in each primitive-buffer.
 * All of unchanged parameters are set here. */	
/* DB	*db;	primitive buffer*/
static void init_prim(DB *db)
{
	/* 16x16 ball texture pattern*/
	extern u_long	ball16x16[];	
	
	/* ball CLUT (16 colorsx32)*/
	extern u_long	ballclut[][8];	

	SPRT_16	*bp;
	u_short	clut;
	int	depth, x, y;

	/* Translate 4bit-texture pattern to (640, 0) on the frame-buffer
	 * and set the default texture page. */	 
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);

	/* background clear enable */
	db->draw.isbg = 1;
	/* set background color*/
	setRGB0(&db->draw, 60, 120, 120);	

	/* Initialize 8x8x8=256 sprites at this point.
	 * CLUT of each texture is changed accroding to the sprites' depth. */	 
	for (depth = 0; depth < 8; depth++) {
		/* load CLUT on frame buffer (notice the CLUT address) */
		clut = LoadClut(ballclut[depth], 0, 480+depth);

		/* Unchanged members of primitives are set here  */
		for (y = 0; y < 8; y++) 
			for (x = 0; x < 8; x++) {
				bp = &db->ball[depth][y][x];
			
				/* SPRT_16 primitve*/
				SetSprt16(bp);		
			
				/* ShadeTex disable*/
				SetShadeTex(bp, 1);	
			
				/* (u0,v0) = (0, 0) */
				setUV0(bp, 0, 0);	
			
				/* set texture CLUT ID*/
				bp->clut = clut;	
			}
	}
}

/* analyze controller*/
/* int	*dx;	sprite coordinate value keyword (X)*/
/* int	*dy;	sprite coordinate value keyword (Y)*/

static int pad_read(int *dx, int *dy)
{
	u_long	padd;	

	/* get controller value*/
	padd = PadRead(1);

	/* exit program*/
	if (padd & PADselect)	return(-1);

	/* move sprite position*/
	if (padd & PADLup)	(*dy)--;
	if (padd & PADLdown)	(*dy)++;
	if (padd & PADLleft)	(*dx)--;
	if (padd & PADLright)	(*dx)++;

	return(0);
}

