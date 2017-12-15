/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			tuto3: simple cube
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/*		  	  Draw 3D objects (cube)  */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

/* libgte functions return 'otz', which indicates the OT linking position.
 * 'otz' is a quater value of the Z in the screen cordinate, and libgte
 * has 15bit (0-0x7fff) z range. Therefore maximum OT size should be 4096
 * (2^14) */
#define SCR_Z	(512)		/* screen depth*/
#define	OTSIZE	(4096)		/* OT size*/

typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment*/
	u_long		ot[OTSIZE];	/* OT*/
	POLY_F4		s[6];		/* cube surface*/
} DB;

static int pad_read(MATRIX *rottrans);
static void init_prim(DB *db, CVECTOR *c);

void tuto3(void)
{
	/* double buffer*/
	DB		db[2];		
	
	/* current db*/
	DB		*cdb;		
	
	/* rotation-translation matrix*/
	MATRIX		rottrans;	
	
	/* color of cube*/
	CVECTOR		col[6];		
	
	int		i;		/* work */
	int		dmy, flg;	/* work */

	/* initialize environment for double buffer (interlace) */
	/*	buffer #0	(0,  0)-(640,480)
	 *	buffer #1	(0,  0)-(640,480)
	 */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* set surface colors */
	for (i = 0; i < 6; i++) {
		col[i].r = rand();	/* R */
		col[i].g = rand();	/* G */
		col[i].b = rand();	/* B */
	}

	/* set primitive parameters on buffer #0/#1 */
	init_prim(&db[0], col);	
	init_prim(&db[1], col);	

	/* enable to display*/
	SetDispMask(1);			

	/*
	 * When using interrace mode, there is no need to changing 
	 * draw/display environment for every frames because the same
	 * same area is used for both drawing and displaying images.
	 * Therefore, the environment should be set only at the first time.
	 * Even in this case, 2 primitive buffers are needed since drawing
	 * process runs parallely with CPU adn GPU. */
	PutDrawEnv(&db[0].draw);	/* set up rendering environment*/
	PutDispEnv(&db[0].disp);	/* set up display environment*/
	
	/* loop while [select] key*/
	while (pad_read(&rottrans) == 0) {	

		/* swap double buffer*/
		cdb = (cdb==db)? db+1: db;	
		
		/* clear OT.
		 * ClearOTagR() clears OT as reversed order. This is natural
		 * for 3D type application, because OT pointer to be linked
		 * is simply related to Z value of the primivie. ClearOTagR()
		 * is faster than ClearOTag because it uses hardware DMA
		 * channel to clear. */
		ClearOTagR(cdb->ot, OTSIZE);	

		/* add cube int OT*/
		add_cubeF4(cdb->ot, cdb->s, &rottrans);

		/* When using interlaced single buffer, all drawing have to be
		 * finished in 1/60 sec. Therefore we have to reset the drawing
		 * procedure at the timing of VSync by calling ResetGraph(1)
		 * instead of DrawSync(0) */
		VSync(0);	
		ResetGraph(1);	

		/* clear background*/
		ClearImage(&cdb->draw.clip, 60, 120, 120);
		
		/* Draw Otag.
		 * Since ClearOTagR() clears the OT as reversed order, the top
		 * pointer of the table is ot[OTSIZE-1]. Notice that drawing
		 * start point is not ot[0] but ot[OTSIZE-1]. */
		/*DumpOTag(cdb->ot+OTSIZE-1);	/* for debug */
		DrawOTag(cdb->ot+OTSIZE-1);	
		FntFlush(-1);
	}
        /* close controller*/
	DrawSync(0);
	return;
}

/* 
 *  Analyzing PAD and setting Matrix */
/* MATRIX *rottrans */
static int pad_read(MATRIX *rottrans)
{
	
	/* angle of rotation*/
	static SVECTOR	ang  = { 0, 0, 0};	
	
	/* translation vertex*/
	static VECTOR	vec  = {0, 0, SCR_Z};	

	/* read from controller*/
	u_long	padd = PadRead(1);
	
	int	ret = 0;

	/* quit*/
	if (padd & PADselect) 	ret = -1;	

	/* change rotation angle*/
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* distance from screen */
	if (padd & (PADL2|PADR2))	vec.vz += 8;
	if (padd & (PADL1|PADR1))	vec.vz -= 8;

	/* calculate matrix*/
	RotMatrix(&ang, rottrans);	/* rotation*/
	TransMatrix(rottrans, &vec);	/* translation*/

	/* set matrix*/
	SetRotMatrix(rottrans);		/* rotation*/
	SetTransMatrix(rottrans);	/* translation*/

	/* print status*/
	FntPrint("tuto3: simple cube angle=(%d,%d,%d)\n",
		 ang.vx, ang.vy, ang.vz);
		
	return(ret);
}

/*
 *	Initialization of Primitives */
/*DB	*db;	/* primitive buffer*/
/*CVECTOR *c;	/* coloer of cube surface*/
static void init_prim(DB *db, CVECTOR *c)
{
	int	i;

	/* initialize for side polygon*/
	for (i = 0; i < 6; i++) {
		/* initialize POLY_FT4*/
		SetPolyF4(&db->s[i]);	
		setRGB0(&db->s[i], c[i].r, c[i].g, c[i].b);
	}
}
