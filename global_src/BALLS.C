/* $PSLibId: Run-time Library Release 4.4$ */
/*			balls: draw many balls
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Primitive Buffer
 */
#define MAXOBJ		3000	
#define OTSIZE		4
typedef struct {		
	u_long		ot[OTSIZE];		/* ordering table */
	DR_MODE		mode;			/* mode */
	SPRT_16		sprt[MAXOBJ];		/* 16x16 fixed-size sprite */
} DB;

/*
 * Position Buffer
 */
typedef struct {		
	u_short x, y;			/* current point */
	u_short dx, dy;			/* verocity */
} POS;

/*
 * Limitations
 */
#define	FRAME_X		320		/* frame size (320x240) */
#define	FRAME_Y		240
#define WALL_X		(FRAME_X-16)	/* reflection point */
#define WALL_Y		(FRAME_Y-16)

static void init_prim(DB *db);	/* preset unchanged primitive members */
static int  pad_read(int n);	/* parse controller */
static int  init_point(POS *pos);/* initialize position table */

static POS	pos[MAXOBJ];
static DB	db[2];		/* double buffer */
void balls_init(void)
{
	init_prim(&db[0]);	/* initialize primitive buffers #0 */
	init_prim(&db[1]);	/* initialize primitive buffers #1 */
	init_point(pos);	/* set initial geometries */
}	

u_long *balls(int nobj, int fx, int fy)
{
	static DB	*cdb;		/* current double buffer */
	
	int	wx = fx-16;
	int	wy = fy-16;
	u_long	*ot;		/* current OT */
	SPRT_16	*sp;		/* work */
	POS	*pp;		/* work */
	int	i, cnt, x, y;	/* work */
	
	
	cdb  = (cdb==db)? db+1: db;	/* swap double buffer ID */
	ClearOTag(cdb->ot, OTSIZE);	/* clear ordering table */
		
	ot = cdb->ot;
	sp = cdb->sprt;
	pp = pos;
	for (i = 0; i < nobj; i++, sp++, pp++) {
		if ((x = (pp->x += pp->dx) % wx*2) >= wx)
			x = wx*2 - x;
		if ((y = (pp->y += pp->dy) % wy*2) >= wy)
			y = wy*2 - y;
			
		setXY0(sp, x, y);	/* update vertex */
		AddPrim(ot, sp);	/* apend to OT */
	}
	AddPrim(ot, &cdb->mode);
	return(ot);
}

/*
 * Initialize drawing Primitives
 */
static void init_prim(DB *db)
{
	extern u_long	ball16x16[];	
	extern u_long	ballclut[][8];	
	
	u_short		tpage;			/* texture page */
	u_short		clut[32];		/* CLUT entry */
	SPRT_16		*sp;			/* work */
	int		i;			/* work */
	
	/* load texture pattern and CLUT */
	tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
	SetDrawMode(&db->mode, 1, 1, tpage, 0);
	
	for (i = 0; i < 32; i++) 
		clut[i] = LoadClut(ballclut[i], 0, 480+i);

	
	/* init sprite */
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		SetSprt16(sp);			/* set SPRT_16 primitve ID */
		SetSemiTrans(sp, 0);		/* semi-amibient is OFF */
		SetShadeTex(sp, 1);		/* shading&texture is OFF */
		setUV0(sp, 0, 0);		/* texture point is (0,0) */
		sp->clut = clut[i%32];		/* set CLUT */
	}
}	

/*
 * Initialize sprite position and verocity
 */
static init_point(POS *pos)
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();
		pos->y  = rand();
		pos->dx = (rand() % 4) + 1;
		pos->dy = (rand() % 4) + 1;
		pos++;
	}
}
