/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* 			tuto8: 1D scroll
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/*		    Sample of 1-D BG scroll */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

/* depth of OT is 8*/
#define OTSIZE		8

/* 
 * Sprite primiteives have no texture page member. 
 * We define new primitive type which has a texture pages. */
typedef struct {
	DR_MODE	mode;	/* mode primitive*/
	SPRT	sprt;	/* sprite primitive*/
} TSPRT;

/* Structures which contains a information about texture location. */

typedef struct {
	/* texture address on main memory*/
	u_long	*addr;	
	
	/* texture addres on frame buffer*/
	short	tx, ty;	
	
	/* texture address on frame buffer*/
	short	cx, cy;	
	
	/* texture page ID*/
	u_short	tpage;	
	
	/* texture CLUT ID*/
	u_short	clut;	
} TEX;

/* primitive double buffer */
typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment*/
	u_long		ot[OTSIZE];	/* OT*/
	TSPRT		far0, far1;	/* far mountains*/
	TSPRT		near0, near1;	/* near mountains*/
	TSPRT		window;		/* train window*/
} DB;

static int update(DB *db,int padd);
static void add_sprite(u_long *ot,TSPRT *tsprt,
			int x0,int y0,int u0,int v0,int w,int h,TEX *tex);
static int SetTSprt(TSPRT *tsprt,int dfe,int dtd,int tpage,RECT *tw);

void tuto8(void)
{
	/* packet double buffer*/
	DB		db[2];		
							  
	/* current burrer*/
	DB		*cdb;		
	
	/* controller data*/
	int	padd;		

	/*  transfer texture data to frame buffer*/
	load_tex();		

	/* set double buffer environment*/
	/*	buffer #0:	(0,  0)-(255,239) (256x240)
	 *	buffer #1:	(0,240)-(255,479) (256x240)
	*/
	SetDefDrawEnv(&db[0].draw, 0,   0, 256, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 256, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 256, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 256, 240);
	db[0].draw.isbg = db[1].draw.isbg = 1;

	SetDispMask(1);		/* start displaying */

	while (((padd = PadRead(1)) & PADselect) == 0) {

		/* swap double buffer ID*/
		cdb = (cdb==db)? db+1: db;	

		/* update coordinates*/
		update(cdb, padd);		

		DrawSync(0);	/* wait for end of drawing*/
		VSync(0);	/* wait for VBLNK*/

		/* swap double buffer*/
		PutDrawEnv(&cdb->draw);	/* update DRAWENV*/
		PutDispEnv(&cdb->disp);	/* update DISPENV*/
		DrawOTag(cdb->ot);	/* draw*/
	}

	DrawSync(0);
	return;
}

/* 
 * 5 8bit texture pages are used here.
 * Both Polygons used for far-landscape and close-landscape use 256x256
 * These pages are used for far landscae, near landescape and train window.
 * 2 pages (512x256) are used for each landscape. 1 page is used for
 * train window.
 * Each landscape is rap-rounded by 512 pixel.
 * Details of the sprite is as follows */
extern u_long far0[];		/* far mountains(left)*/
extern u_long far1[];		/* far mountains(right)*/
extern u_long near0[];		/* near mountains(left)*/
extern u_long near1[];		/* near mountains(right)*/
extern u_long window[];		/* train window*/

/* define ID to every textures*/
#define TEX_FAR0	0
#define TEX_FAR1	1
#define TEX_NEAR0	2
#define TEX_NEAR1	3
#define TEX_WINDOW	4

/* Initial value of the texture structure*/
static TEX tex[] = {
	/*addr   tx   ty cx   cy tpage clut			*/
	/*--------------------------------------------------------------*/ 
	far0,   512,   0, 0, 481,    0,   0,	/* far0    */ 
	far1,   512, 256, 0, 482,    0,   0,	/* far1    */ 
	near0,  640,   0, 0, 483,    0,   0,	/* near0   */ 
	near1,  640, 256, 0, 484,    0,   0,	/* near1   */ 
	window, 768,   0, 0, 485,    0,   0,	/* window  */ 
	0,					/* terminator*/
};

/* Load all texture data to V-RAM at once. */
load_tex(void)
{
	int	i;

	/* loop while addr is not 0 */
	for (i = 0; tex[i].addr; i++) {	

		/* Load texture pattern */
		tex[i].tpage = LoadTPage(tex[i].addr+0x80, 1, 0,
					 tex[i].tx, tex[i].ty,  256, 256);
		
		/* Load texture clut */
		tex[i].clut = LoadClut(tex[i].addr, tex[i].cx, tex[i].cy);
	}
}

/* Read controller and update the scroll parameters* */

/* DB	*db,	/* primitive buffer */
/* int	padd	/* controller data */
static int update(DB *db, int padd)
{
	
	static int 	ofs_x = 0;	/* position*/
	static int	v     = 0;	/* verosicy*/
	
	/* texture*/
	TEX	*tp[2];			
	
	/* direction 0:right to left, 1:reverse */
	int	side;			
	
	/* flag to disaplay near mountains */
	int	isnear   = 1;		
	
	/* flag to display window */
	int	iswindow = 1;		
	
	int	d;

	/* update speed*/
	if (padd & PADLright)	v++;	/* speed up*/
	if (padd & PADLleft)	v--;	/* speed down*/
	ofs_x += v;			/* total amount*/

	/* if needed, it is surpressed to display landscapes or a window.   */
	if (padd & (PADR1|PADR2))	isnear   = 0;
	if (padd & (PADL1|PADL2))	iswindow = 0;

	/* clear OT*/
	ClearOTag(db->ot, OTSIZE);

	/* display far landscape. (The period of rap-round is 512 pixel)  */
    display_far:

	side = (ofs_x/4)&0x100;	/* decide L->R or R->L*/
	d    = (ofs_x/4)&0x0ff;	/* limited between 0-256*/

	/* Put TSPRT primite into OT.
	 * The scroll is achived by changing (u0,v0) not (x0, y0).
	 * Left primitive draws the area of (0,0)-(256-d,240)
	 * Right primitive draws the area of (256-d,0)-(256,240)
	 * 'd' is valid according to the  scroll position.
	 * */
	tp[0] = &tex[side==0? TEX_FAR0: TEX_FAR1];	/* right hand */
	tp[1] = &tex[side==0? TEX_FAR1: TEX_FAR0];	/* left hand */

	add_sprite(db->ot, &db->far0, 0,     16, d, 0, 256-d, 168, tp[0]);
	add_sprite(db->ot, &db->far1, 256-d, 16, 0, 0, d,     168, tp[1]);

	/* Display near landscape.( The cycle of lap-round is 512 pixel   */
    display_near:
	if (isnear == 0) goto display_window;

	side = (ofs_x/2)&0x100;	
	d    = (ofs_x/2)&0x0ff;

	tp[0] = &tex[side==0? TEX_NEAR0: TEX_NEAR1];
	tp[1] = &tex[side==0? TEX_NEAR1: TEX_NEAR0];

	add_sprite(db->ot+1, &db->near0, 0,     32, d, 0, 256-d, 168, tp[0]);
	add_sprite(db->ot+1, &db->near1, 256-d, 32, 0, 0, d,     168, tp[1]);

	/* draw window*/
    display_window:
	if (iswindow == 0) return;

	add_sprite(db->ot+2,
		   &db->window, 0, 0, 0, 0, 256, 200, &tex[TEX_WINDOW]);
}

/* Put TSPR primitive into OT */
/* u_long *ot,	  /* OT */
/* TSPRT  *tsprt, /* TSPRT primitive */
/* int	  x0,y0	  /* left-upper corner of TSPRT */
/* int	  u0,v0	  /* left-upper corner of TSPRT(U) */
/* int	  w,h	  /* size of texture pattern */
/* TEX	  *tex	  /* texture attributes */

static void add_sprite(u_long *ot,TSPRT *tsprt,
			int x0,int y0,int u0,int v0,int w,int h,TEX *tex)
{
	/* initialize TSPRT primitive*/
	SetTSprt(tsprt, 1, 1, tex->tpage, 0);

	/* shading off*/
	SetShadeTex(&tsprt->sprt, 1);	
	
	/* set left-upper corner point of (x,y)*/
	setXY0(&tsprt->sprt, x0, y0);	
	
	/* set left-upper corner point of (u,v)*/
	setUV0(&tsprt->sprt, u0, v0);	
	
	/* set size of sprite*/
	setWH(&tsprt->sprt, w, h);	
	
	/* set CLUT ID*/
	tsprt->sprt.clut = tex->clut;	

	/* add to OT*/
	AddPrim(ot, &tsprt->mode);	
}

/* TSPRT *tsprt, TSPRT pointer*/
/* int	 dfe,	 /* same as DRAWENV.dfe */
/* int	 dtd,	 /* same as DRAWENV.dtd */
/* int	 tpage,	 /* texture page */
/* RECT	 *tw	 /* texture window */

static int SetTSprt(TSPRT *tsprt,int dfe,int dtd,int tpage,RECT *tw)
{
	/* initialize MODE primitive*/
 	SetDrawMode(&tsprt->mode, dfe, dtd, tpage, tw);

	/* initialize SPRT primitive*/
	SetSprt(&tsprt->sprt);

	/* Merge 2 primitives to one.
	 * Merging primitives should be located on the continuous
	 * memory area.
	 * The total size of 2 primitives must be under 16 words. */
	if (MargePrim(&tsprt->mode, &tsprt->sprt) != 0) {
		printf("Marge failed!\n");
		return(-1);
	}
	return(0);
}

