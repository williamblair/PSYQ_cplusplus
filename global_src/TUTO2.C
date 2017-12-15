/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			tuto2: RotTransPers
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/*		  Draw rotating plate using GTE functions */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define SCR_Z	512		/* distant to screen*/

/*#define DEBUG*/

/* primitive buffer*/
typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment*/
	u_long		ot;		/* ordering table*/
	POLY_G4		wall;		/* primitive*/
} DB;

static int pad_read(void);
static void init_prim(DB *db);

void tuto2(void)
{
	SVECTOR	x[4];		/* walls position*/
	DB	db[2];		/* primitive double buffer */
	DB	*cdb;		/* current DB */
	long	dmy, flg;	/* work */
	int	i;

	/* initialize wall position*/
	setVector(&x[0], -128, -128, 0); setVector(&x[1],  128, -128, 0);
	setVector(&x[2], -128,  128, 0); setVector(&x[3],  128,  128, 0);

	/* initialize environment for double buffer  */
	/*	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,240)-(320,480) (320x240)
	 */
	init_system(160, 120, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* init primitives*/
	init_prim(&db[0]);
	init_prim(&db[1]);

	/* display*/
	SetDispMask(1);

	/* main loop*/
	while (pad_read() == 0) {

		FntPrint("tuto2: use GTE\n");

		/* swap double buffer ID*/
		cdb = (cdb==db)? db+1: db;	

		/* clea ordering table*/
		ClearOTag(&cdb->ot, 1);		

		/* rotate and shift each vertex of the wall */
                RotTransPers(&x[0], (long *)&cdb->wall.x0, &dmy, &flg);
                RotTransPers(&x[1], (long *)&cdb->wall.x1, &dmy, &flg);
                RotTransPers(&x[2], (long *)&cdb->wall.x2, &dmy, &flg);
                RotTransPers(&x[3], (long *)&cdb->wall.x3, &dmy, &flg);

#ifdef DEBUG
		dumpXY4(&cdb->wall);
#endif
		/* add to OT*/
		AddPrim(&cdb->ot, &cdb->wall);		

		DrawSync(0);	/* wait for end of drawing*/
		VSync(0);	/* wait for the next VBLNK*/

		/* swap double buffer */
		
		PutDrawEnv(&cdb->draw); /* update DRAWENV*/
		PutDispEnv(&cdb->disp); /* update DISPENV*/
#ifdef DEBUG
		DumpOTag(&cdb->ot);
#endif
		DrawOTag(&cdb->ot);	/* draw*/
		FntFlush(-1);		/* flush print buffer */
	}
	DrawSync(0);
	return;
}

/* analyzie controller and set matrix for rotation and translation. */
static int pad_read(void)
{
	/* Because PlayStation treats angles like that :360 degree = 4096,
	 * you need to set angles as follows */
	/*	 45 deg = 512*/
	/* rotation angle*/
	static SVECTOR	ang   = {512, 512, 512};	
	
	/* translate vector*/
	static VECTOR	vec   = {0,     0, SCR_Z};	
	
	/* work matrix */
	static MATRIX	m;				

	int	ret = 0;
	u_long	padd;
	
	/* get controller status*/
	padd = PadRead(1);

	/* quit from program*/
	if (padd & PADselect) 	ret = -1;

	/* change value of rotation angle*/
	if (padd & PADRup)		ang.vx += 32;
	if (padd & PADRdown)		ang.vx -= 32;
	if (padd & PADRleft) 		ang.vy += 32;
	if (padd & PADRright)		ang.vy -= 32;

	/* change scale*/
	if (padd & (PADL2|PADR2))	vec.vz += 8;
	if (padd & (PADL1|PADR1))	vec.vz -= 8;

	ang.vx += 32;

	/* rotate*/
	RotMatrix(&ang, &m);		
	
	/* translate*/
	TransMatrix(&m, &vec);		

	/* set rotation*/
	SetRotMatrix(&m);		
	
	/* set translation*/
	SetTransMatrix(&m);		

	return(ret);
}

/* Initialization of Primitive buffer */
/* DB *db;	primitive buffer*/
static void init_prim(DB *db)
{
	/* auto background clear mode*/
	db->draw.isbg = 1;			
	
	/* set background color*/
	setRGB0(&db->draw, 60, 120, 120);	

	/* initialize 'wall' primitive*/
	SetPolyG4(&db->wall);
	
	/* set colors for each vertex*/
	setRGB0(&db->wall, 0xff, 0xff, 0x00);
	setRGB1(&db->wall, 0xff, 0x00, 0xff);
	setRGB2(&db->wall, 0x00, 0xff, 0xff);
	setRGB3(&db->wall, 0xff, 0x00, 0x00);
}

