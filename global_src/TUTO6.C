/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			tuto6: cube with depth-queue
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/*		    rotating cubes with depth queue */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define SCR_Z	(512)		
#define	OTSIZE	(4096)

/* start point of fog effect*/
#define FOGNEAR (300)		
					   
/* primitive buffer*/
typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment*/
	u_long		ot[OTSIZE];	/* OT*/
	POLY_F4		s[6];		/* sides of cube*/
} DB;

static int pad_read(MATRIX *rottrans);
static void init_prim(DB *db);

void tuto6(void)
{
	DB	db[2];		/* double buffer */
	DB	*cdb	;	/* current buffer */
	MATRIX	rottrans;	/* rot-trans matrix */
	CVECTOR	col[6];		/* color of cube surface */
	
	int	i;		/* work */
	int	dmy, flg;	/* dummy */

	/* initialize environment for double buffer (interlace) */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* FarColor have to be the same as background*/
	SetFarColor(60,120,120);	
	
	/* start point of depth quweue*/
	SetFogNear(FOGNEAR,SCR_Z);	

	/* set primitive parametes on buffer*/
	init_prim(&db[0]);	/* buffer #0 */
	init_prim(&db[1]);	/* buffer #1 */

	/* set surface colors*/
	for (i = 0; i < 6; i++) {
		col[i].cd = db[0].s[0].code;	/* code */
		col[i].r = rand();		/* R */
		col[i].g = rand();		/* G */
		col[i].b = rand();		/* B */
	}

	SetDispMask(1);		/* start displaying*/

	PutDrawEnv(&db[0].draw);
	PutDispEnv(&db[0].disp);

	while (pad_read(&rottrans) == 0) {

		cdb = (cdb==db)? db+1: db;	
		ClearOTagR(cdb->ot, OTSIZE);	
		
		/* apend cubes into OT*/
		add_cubeF4F(cdb->ot, cdb->s, &rottrans, col);

		VSync(0);
		ResetGraph(1);
		
		/* clear background*/
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		/* draw*/
		DrawOTag(cdb->ot+OTSIZE-1);
		FntFlush(-1);
	}
	DrawSync(0);
	return;
}

/* 
 * Analyzing PAD and Calcurating Matrix */
/* MATRIX *rottrans; 	/* rot-trans matrix */
static int pad_read(MATRIX *rottrans)
{
	/* angle of rotation for the cube*/
	static SVECTOR	ang  = { 0, 0, 0};	
	
	/* translation vector*/
	static VECTOR	vec  = {0, 0, SCR_Z};	

	/* read from controller*/
	u_long	padd = PadRead(1);

	int	ret = 0;
	

	/* quit program*/
	if (padd & PADselect) 	ret = -1;

	/* change the rotation angles for the cube and the light source */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* distance from screen */
	if (padd & (PADL2|PADR2))	vec.vz += 8;
	if (padd & (PADL1|PADR1))	vec.vz -= 8;

	/* matrix calcuration*/
	/* rotation angle of the cube*/
	RotMatrix(&ang, rottrans);	
	
	/* translation vector of the cube*/
	TransMatrix(rottrans, &vec);	
	
	FntPrint("tuto6: fog angle=(%d,%d,%d)\n",
		 ang.vx, ang.vy, ang.vz);
	return(ret);
}

/*
 *	Initialization assosiate with Primitives. */
/* DB	*db;	/* primitive buffer */
static void init_prim(DB *db)
{
	int i;

	for(i = 0;i < 6; i++)
		SetPolyF4(&db->s[i]);
}
