/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			tuto4: cube with lighting
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/*		Draw 3D objects (cube) with lighting */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define SCR_Z	(512)		
#define	OTSIZE	(4096)

/* primitive buffer*/
typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment*/
	u_long		ot[OTSIZE];	/* OT*/
	POLY_F4		s[6];		/* sides of cube*/
} DB;

/* light source Local Color Matrix*/
static MATRIX	cmat = {
/* light source    #0, #1, #2, */
		ONE*3/4,  0,  0, /* R */
		ONE*3/4,  0,  0, /* G */
		ONE*3/4,  0,  0, /* B */
};

/* light vector (local light matrix) */
static MATRIX lgtmat = {
	/*          X     Y     Z */
	          ONE,  ONE, ONE,	/* light source #0*/
		    0,    0,    0,	/*      #1 */
		    0,    0,    0	/*      #2 */
};

static int pad_read(MATRIX *rottrans, MATRIX *rotlgt);
static void init_prim(DB *db);

void tuto4()
{
	DB	db[2];		/* double buffer */
	DB	*cdb	;	/* current buffer */
	MATRIX	rottrans;	/* rot-trans matrix */
	
	/* rot-trans matrix for light source*/
	MATRIX		rotlgt;		
	
	/* lighint matrix*/
	MATRIX		light;

	/* color of cube surface*/
	CVECTOR		col[6];	
	
	int		i;		/* work */
	int		dmy, flg;	/* dummy */

	/* initialize environment for double buffer (interlace) */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* set background color*/
	SetBackColor(64, 64, 64);	
	
	/* set local color matrix*/
	SetColorMatrix(&cmat);		

	/* set primitive parametes on buffer*/
	init_prim(&db[0]);	
	init_prim(&db[1]);	

	/* set surface colors. 
	 * NormalColorCol() overwrite the 'code' field of the primitive.
	 * So set the code here. */
	for (i = 0; i < 6; i++) {
		col[i].cd = db[0].s[0].code;	/* CODE */
		col[i].r  = rand();		/* R */
		col[i].g  = rand();		/* G */
		col[i].b  = rand();		/* B */
	}

	SetDispMask(1);			/* start displaying*/
	PutDrawEnv(&db[0].draw);	/* set DRAWENV*/
	PutDispEnv(&db[0].disp);	/* set DISPENV*/

	while (pad_read(&rottrans, &rotlgt) == 0) {

		cdb = (cdb==db)? db+1: db;	/* change current buffer */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear OT */

		/* Calcurate Matrix for the light source  */
		MulMatrix0(&lgtmat, &rotlgt, &light);
		SetLightMatrix(&light);

		/* apend cubes into OT*/
		add_cubeF4L(cdb->ot, cdb->s, &rottrans, col);

		VSync(0);
		ResetGraph(1);
		
		/* clear background*/
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		/* draw primitives*/
		/*DumpOTag(cdb->ot+OTSIZE-1);	/* for debug */
		DrawOTag(cdb->ot+OTSIZE-1);	
		FntFlush(-1);
	}
	DrawSync(0);
	return;
}

/* 
 * Analyzing PAD and Calcurating Matrix */
/* MATRIX *rottrans; 	/* rot-trans matrix*/
/* MATRIX *rotlgt;	/* light source matrix*/
static int pad_read(MATRIX *rottrans, MATRIX *rotlgt)
{
	/* angle of rotation for the cube*/
	static SVECTOR	ang  = { 0, 0, 0};	
	
	/*angle of rotation for the light source */
	static SVECTOR	lgtang = {1024, -512, 1024};	
	
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
	
	/* change the rotation angles only for the light source  */
	if (padd & PADLup)	lgtang.vx += 32;
	if (padd & PADLdown)	lgtang.vx -= 32;
	if (padd & PADLleft) 	lgtang.vy += 32;
	if (padd & PADLright)	lgtang.vy -= 32;

	/* distance from screen */
	if (padd & (PADL2|PADR2))	vec.vz += 8;
	if (padd & (PADL1|PADR1))	vec.vz -= 8;

	/* rotation matrix of the light source*/
	RotMatrix(&lgtang, rotlgt);	
	MulMatrix(rotlgt, rottrans);

	/* set rot-trans-matrix of the cube*/
	RotMatrix(&ang, rottrans);	
	TransMatrix(rottrans, &vec);	

	FntPrint("tuto4: lighting angle=(%d,%d,%d)\n",
		 lgtang.vx, lgtang.vy, lgtang.vz);
	return(ret);
}

/*
 *	Initialization assosiate with Primitives. */
/* DB	*db;	/* primitive buffer */
static void init_prim(DB *db)
{
	int i;

	/* Flat shading 4 point Polygon declared */
	for(i = 0;i < 6;i++)
		SetPolyF4(&db->s[i]);	
}
