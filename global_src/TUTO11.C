/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*		 tuto11: special effect (mosaic)
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/*		               Mosaic */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define NCELL_X	16		/* number of cells*/
#define	NCELL_Y	16

typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment*/
	u_long		ot;		/* OT*/
	POLY_FT4	bg[NCELL_X*NCELL_Y];

} DB;

static void mozaic(u_long *ot, POLY_FT4 *ft4, int ox, int oy, int dx, 
		int dy, int ou, int ov, int du, int dv,
		int divx, int divy, int rate);
void tuto11(void)
{
	/* BG texture pattern*/
	extern	u_long	bgtex[];	
	
	DB		db[2];		/* primitive double buffer */
	DB		*cdb;		/* current buffer */
	SVECTOR		x0;		/* positoin */
	u_short		tpage;		/* texture page */
	u_short		clut;		/* texture CLUT */
	int		id, i, j;	/* work */
	u_long		padd;
	
	/* mosaic rate (0-7)*/
	int		mrate = 0;	

	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));
	
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* load texture page*/
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256,256);
	clut  = LoadClut(bgtex, 0,500);

	for (i = 0; i < 2; i++) {
		/* set backgound color */
		db[i].draw.isbg = 1;
		setRGB0(&db[i].draw, 0, 0, 0);	

		/* initialize POLY_FT4*/
		for (j = 0; j < NCELL_X*NCELL_Y; j++) {
			SetPolyFT4(&db[i].bg[j]);
			SetShadeTex(&db[i].bg[j], 1);
			db[i].bg[j].tpage = tpage;
			db[i].bg[j].clut  = clut;
		}
	}

	/* set sprite initial postion*/
	setVector(&x0, 0,   0, 0);

	/* main loop*/
	SetDispMask(1);
	while (1) {

		FntPrint("tuto11: mosaic\n");
		
		/* initialize primitive buffer*/
		cdb = (cdb==db)? db+1: db;
		ClearOTag(&cdb->ot, 1);

		/* read controller*/
		padd = PadRead(1);

		if (padd & PADselect) 	break;
		if (padd & PADRup)	mrate++;
		if (padd & PADRdown)	mrate--;
		if (padd & PADLright)	x0.vx++;
		if (padd & PADLleft)	x0.vx--;
		if (padd & PADLup)	x0.vy--;
		if (padd & PADLdown)	x0.vy++;

		/* mosaic*/
		limitRange(mrate, 0, 8);
		mozaic(&cdb->ot, cdb->bg,
				x0.vx, x0.vy, 256, 256, 0, 0, 256, 256,
				NCELL_X, NCELL_Y, mrate);

		/* draw*/
		DrawSync(0);	
		VSync(0);	

		PutDrawEnv(&cdb->draw); 
		PutDispEnv(&cdb->disp); 
		DrawOTag(&cdb->ot);	
		FntFlush(-1);
	}
	DrawSync(0);
	return;
}

/*
 *	Example for MOSAIC */
/* u_long *ot,			
/* POLY_FT4 *ft4,	/* primitive*/
/* int ox, oy,		/* left-upper corner*/
/* int dx, dy		/* size of primitive*/
/* int ou, ov		/* left-upper corner of texture*/
/* int du, dv,	 	/* texture size*/
/* int divx, divy,	/* number of division*/
/* int rate		/* mosaic rate*/
static void mozaic(u_long *ot, POLY_FT4 *ft4, int ox, int oy, int dx, 
		int dy, int ou, int ov, int du, int dv,
		int divx, int divy, int rate)
{
	
	/* unit for mosic*/
	int	sx = dx/divx, sy = dy/divx;	
	int	su = du/divy, sv = du/divy; 	
	
	/* loop counter*/
	int	ix, iy;				
	
	int	x, y, u, v;			/* work */
	int	u0, v0, u1, v1;			/* work */

	/* mosic by changing (u,v) value of each primitive */
	for (v = ov, y = oy, iy = 0; iy < divy; iy++, v += sv, y += sy) {
		for(u = ou, x = ox, ix = 0; ix < divx; ix++, u += su, x += sx){

			/* calcrating (u, v)  
			 * The essense management of mosaic is here. */
			u0 = u + rate;		v0 = v + rate;
			u1 = u+su - rate;	v1 = v+sv - rate;

			/* check overflow*/
			if (u1 >= 256)	u1 = 255;
			if (v1 >= 256)	v1 = 255;

			/* set values for primitive  */
			setUV4(ft4, u0, v0, u1, v0, u0, v1, u1, v1);
			setXYWH(ft4, x, y, sx, sy);

			AddPrim(ot, ft4);	/* add to OT*/
			ft4++;			
		}
	}
}

