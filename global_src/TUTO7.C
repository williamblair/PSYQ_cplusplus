/* $PSLibId: Run-time Library Release 4.4$ */
/*		  tuto7: many cubes with local coordinates
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/*		      Drawing many 3D objects. */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define SCR_Z	1024		
#define	OTSIZE	4096
#define NCUBE	256		/* max number of cubes*/

typedef struct {
	POLY_F4		s[6];		/* surface of cube*/
} CUBE;

typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment*/
	u_long		ot[OTSIZE];	/* OT*/
	CUBE		c[NCUBE];	/* pointer to CUBE*/
} DB;

typedef struct {
	CVECTOR	col[6];			/* color of cube surface*/
	SVECTOR	trans;			/* translation vector (local coord.) */
} CUBEATTR;

/* light source color (Local Color Matrix)*/
static MATRIX	cmat = {
/*  light source   #0, #1, #2, */
		ONE*3/4,  0,  0, /* R */
		ONE*3/4,  0,  0, /* G */
		ONE*3/4,  0,  0, /* B */
};

/* the vector of the light source (Local light Matrix  */
static MATRIX lgtmat = {
	/*          X     Y     Z */
	          ONE,  ONE, ONE,	/* light source #0*/
		    0,    0,    0,	/* #1 */
		    0,    0,    0	/* #2 */
};

static int pad_read(int *ncube, 
		MATRIX *world, MATRIX *local, MATRIX *rotlgt);
static void init_attr(CUBEATTR *attr, int nattr);
static init_prim(DB *db);

void tuto7(void)
{
	
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current buffer */
	
	/* attribute of cube*/
	CUBEATTR	attr[NCUBE];	
	
	/* world-screen matrix*/
	MATRIX		ws;

	/* local-screen*/
	MATRIX		ls;

	/* light source matrix*/
	MATRIX		lls;		
	
	/* lighint matrix*/
	MATRIX		light;

	/* number of cubes*/
	int		ncube = NCUBE/2;
	
	int		i;		/* work */
	long		dmy, flg;	/* dummy */

	/* set double buffer environment */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* set background color*/
	SetBackColor(64, 64, 64);	
	
	/* set local
	   color matrix*/
	SetColorMatrix(&cmat);		

	/* initialize primitive buffer*/
	init_prim(&db[0]);
	init_prim(&db[1]);
	init_attr(attr, NCUBE);

	/* display enable*/
	SetDispMask(1);		

	PutDrawEnv(&db[0].draw); /* set DRAWENV*/
	PutDispEnv(&db[0].disp); /* set DISPENV*/

	while (pad_read(&ncube, &ws, &ls, &lls) == 0) {

		cdb = (cdb==db)? db+1: db;	
		ClearOTagR(cdb->ot, OTSIZE);	

		/* Calcurate Matrix for the light source;
		 * Notice that MulMatrix() destroys current matrix. */
		PushMatrix();
		MulMatrix0(&lgtmat, &lls, &light);
		PopMatrix();
		SetLightMatrix(&light);
		
		/* put the primitives of cubes int OT*/
		limitRange(ncube, 1, NCUBE);
		
		/* add cubes to the OT
		 * note that only translation vector of the local
		 * screen matrix is changed  */ 
		for (i = 0; i < ncube; i++) {
			RotTrans(&attr[i].trans, (VECTOR *)ls.t, &flg);
			add_cubeF4L(cdb->ot, cdb->c[i].s, &ls, attr[i].col);
		}

		VSync(0);
		ResetGraph(1);

		/* clear background*/
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		DrawOTag(cdb->ot+OTSIZE-1);	/* draw*/
		FntFlush(-1);			/* for debug */
	}
	DrawSync(0);
	return;
}


/* Analyzing PAD and Calcurating Matrix */
/* int	  *ncube	/* number of cubes */
/* MATRIX *ws	 	/* rottrans matrix for all */
/* MATRIX *ls	 	/* rottrans matrix for each */
/* MATRIX *lls		/* light local-screen matrix */
static int pad_read(int *ncube, 
		MATRIX *ws, MATRIX *ls, MATRIX *lls)
{
	/* PlayStation treats angle like as follows: 360 degree = 4096 */
	
	/* Be careful that the rotation of an object, and the rotation of the world outside the object in the reverse direction, are equivalent. */
	 
	/*  rotation angle of world*/
	static SVECTOR	wang    = {0,  0,  0};	
	
	/* rotation angle for each cube*/
	static SVECTOR	lang   = {0,  0,  0};	
	
	/* lotation angle for light source*/
	static SVECTOR	lgtang = {1024, -512, 1024};	
	
	/* translation vector for all objects*/
	static VECTOR	vec    = {0,  0,  SCR_Z};

	/* scale of the each cube*/
	static VECTOR	scale  = {1024, 1024, 1024, 0};
	
	SVECTOR	dwang, dlang;
	int	ret = 0;
	u_long	padd = PadRead(0);
	
	/* quit*/
	if (padd & PADselect) 	ret = -1;	

	/* rotate all cubes */
	if (padd & PADRup)	wang.vz += 32;
	if (padd & PADRdown)	wang.vz -= 32;
	if (padd & PADRleft) 	wang.vy += 32;
	if (padd & PADRright)	wang.vy -= 32;

	/* rotate each cube*/
	if (padd & PADLup)	lang.vx += 32;
	if (padd & PADLdown)	lang.vx -= 32;
	if (padd & PADLleft) 	lang.vy += 32;
	if (padd & PADLright)	lang.vy -= 32;
	
	/* distance from screen */
	if (padd & PADR2)	vec.vz += 8;
	if (padd & PADR1)	vec.vz -= 8;

	/* change number of displayed cubes */
	if (padd & PADL2)       (*ncube)++;
	if (padd & PADL1)	(*ncube)--;
	limitRange(*ncube, 1, NCUBE-1);

	FntPrint("objects = %d\n", *ncube);
	
	/* calcurate world-screen matrix */
	RotMatrix(&wang, ws);	
	TransMatrix(ws, &vec);

	/* calcurate matrix for each cubes.
	 * In this case, each local-screen matrix is the same because each
	 * cube rotates to the same direction. */
	RotMatrix(&lang, ls);
	
	/* make local-screen matrix:
	 * Notice the difference between  MulMatrix() and MulMatrix2() */
	MulMatrix2(ws, ls); 
	
	/* make light matrix*/
	RotMatrix(&lgtang, lls);
	MulMatrix(lls, ls);

	/* scale of the local matrix represents the scale of the ls object. */
	ScaleMatrix(ls, &scale);
	
	/*Setting Matrix for all the objects */
	SetRotMatrix(ws);	/* rotation*/
	SetTransMatrix(ws);	/* translation*/

	return(ret);
}

#define MIN_D 	64		/* minumus distance between each cube */
#define MAX_D	(SCR_Z/2)	/* maximum distance */
/* CUBEATTR	*attr,	/* attribute of cube */
/* int		nattr	/* number of cube */
static void init_attr(CUBEATTR *attr, int nattr)
{
	int	i;
	POLY_F4	templ;

	SetPolyF4(&templ);

	for (; nattr; nattr--, attr++) {
		for (i = 0; i < 6; i++) {
			attr->col[i].cd = templ.code;	/* sys code */
			attr->col[i].r  = rand();	/* R */
			attr->col[i].g  = rand();	/* G */
			attr->col[i].b  = rand();	/* B */
		}
		/* Set initial coordinates */
		attr->trans.vx = (rand()/MIN_D*MIN_D)%MAX_D-(MAX_D/2);
		attr->trans.vy = (rand()/MIN_D*MIN_D)%MAX_D-(MAX_D/2);
		attr->trans.vz = (rand()/MIN_D*MIN_D)%MAX_D-(MAX_D/2);
	}
}
	
/* DB	*db;	/* primivie buffer */
static init_prim(DB *db)
{
	int	i, j;

	for (i = 0; i < NCUBE; i++) 
		for (j = 0; j < 6; j++) 
			SetPolyF4(&db->c[i].s[j]);
}
