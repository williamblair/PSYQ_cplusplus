/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			  tuto0: draw sprites
 *
 *	   Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */	
/*		Display 2 moving sprites with sprite animation. */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define DT	4	/* sprite moving unit*/

static void load_texture4(u_short tpage[4], u_short clut[4]);

void tuto0(void)
{
	/* drawing environment and display environment.
	 * Two pair of environments are needed for double buffer */
	DRAWENV		draw[2];	/* drawing environment*/
	DISPENV		disp[2];	/* display environment*/

	POLY_FT4	mat0, mat1;	/* texture mapped polygon */
	SVECTOR		x0, x1;		/* diplay position (vs vy is used) */

	u_short		tpage[4];	/* texture page ID (4pages) */
	u_short		clut[4];	/* texture CLUT ID */
	int		frame = 0;	/* counter*/
	int		w = 64, h = 64;	/* scale*/
	int		u, v;		/* work */
	u_long		padd;		/* work */

	/* initialize debug print message function.
	 * FntLoad(x,y) loads font pattern to (x,y) on the frame buffer.
	 * SetDumpFnt() defines standard on-screen print stream. */
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));
	   
	/* set 2 drawing and display environments to construct frame
	 * double buffer.
	 * When buffer #1 is drawing, buffer #2 is displayed.
	 * When buffer #2 is drawing, buffer #1 is displayed. */
	/*	buffer #1	(0,  0)-(320,240)
	 *	buffer #2	(0,240)-(320,480)
	 */
	SetDefDrawEnv(&draw[0], 0,   0, 320, 240);
	SetDefDispEnv(&disp[0], 0, 240, 320, 240);
	SetDefDrawEnv(&draw[1], 0, 240, 320, 240);
	SetDefDispEnv(&disp[1], 0,   0, 320, 240);

	/* indicate to clear BG*/
	draw[0].isbg = draw[1].isbg = 1;
	setRGB0(&draw[0], 0, 0, 0);	/* (R,G,B)=(0,0,0) */
	setRGB0(&draw[1], 0, 0, 0);	/* (R,G,B)=(0,0,0) */

	/* initialize polygon primiteivs. 
	 * SetPolyFT4() initialize POLY_FT4 primitive.
	 * SetShadeTex() indicates to inhibit ShadeTex option. */
	SetPolyFT4(&mat0);		
	SetShadeTex(&mat0, 1);		/* ShadeTex disable */
	SetPolyFT4(&mat1);
	SetShadeTex(&mat1, 1);		/* ShadeTex disable */

	/* set the initial position of each sprite */
	x0.vx = 80-w/2;  x1.vx = 240-w/2;
	x0.vy = 120-h/2; x1.vy = 120-h/2;

	/* load the texture pattern to frame buffer, and set default tpage
	 * and CLUT   */
	load_texture4(tpage, clut);

	/* display enable*/
	SetDispMask(1);

	/* main loop*/
	while (1) {
		/* swap frame double buffer */
		PutDrawEnv(frame%2? &draw[0]:&draw[1]);
		PutDispEnv(frame%2? &disp[0]:&disp[1]);

		/* get current controller status*/
		padd = PadRead(1);

		/* calculate sprite position*/
		if (padd & PADLup)	x0.vy -= DT;
		if (padd & PADLdown)	x0.vy += DT;
		if (padd & PADLleft) 	x0.vx -= DT;
		if (padd & PADLright)	x0.vx += DT;

		if (padd & PADRup)	x1.vy -= DT;
		if (padd & PADRdown)	x1.vy += DT;
		if (padd & PADRleft) 	x1.vx -= DT;
		if (padd & PADRright)	x1.vx += DT;

		/* scale*/
		if (padd & PADL1)	w--, h--;
		if (padd & PADR1) 	w++, h++;

		/* end animation*/
		if (padd & PADselect) 	break;

		/* draw the sprite (Matrchang).
		 * Texture patterns are changed by each frame to do the  
		 * sprite animations */
 
		/* set texture pages*/
		mat0.tpage = mat1.tpage = tpage[(frame/16)%4];
		mat0.clut  = mat1.clut  = clut[(frame/16)%4];

		/* set texture point (u,v)*/
		u = (frame%4)*64;
		v = ((frame/4)%4)*64;
		setUVWH(&mat0, u, v, 63, 63);
		setUVWH(&mat1, u, v, 63, 63);

		/* calculate sprite position*/
		setXYWH(&mat0, x0.vx, x0.vy, w, h);
		setXYWH(&mat1, x1.vx, x1.vy, w, h);

		/* draw primitives*/
		DrawPrim(&mat0);
		DrawPrim(&mat1);

		/* print debug message*/
		FntPrint("tuto0: simplest sample\n");
		FntPrint("mat0=");dumpXY0(&mat0);
		FntPrint("mat1=");dumpXY0(&mat1);

		/* FntFlush(-1) indicates flush the standard stream */
		FntFlush(-1);
		
		/* wait for V-BLNK*/
		VSync(0);

		/* update frame counter*/
		frame++;
	}
	DrawSync(0);
	return;
}

/*
 * load texture pattern to frame buffer */
/* u_short tpage[4];	texture page ID*/
/* u_short clut[4];	texture CLUT ID*/
static void load_texture4(u_short tpage[4], u_short clut[4])
{
	/* mat0, mat1, mat2, mat3 are Sprite animation image patterns.
	 * Animation patterns use 4 texture pages, and each page contains
	 * 64 patterns (64x64 size)
	 * Image format here is;
	 *	0x000-0x07f	CLUT  (256x2byte entry)
	 *	0x200-		INDEX (4bit mode, size=256x256) */
	extern	u_long	mat0[];		/* Matchang of texture page #0 */
	extern	u_long	mat1[];		/* Matchang of texture page #1 */
	extern	u_long	mat2[];		/* Matchang of texture page #2 */
	extern	u_long	mat3[];		/* Matchang of texture page #3 */

	/* load 4 texture pages.
	 * Since each texture pattern is 4bit mode, it uses 64x256 area
	 * (not 256x256) of the frame buffer.  */
	tpage[0] = LoadTPage(mat0+0x80, 0, 0, 640,  0, 256,256);
	tpage[1] = LoadTPage(mat1+0x80, 0, 0, 640,256, 256,256);
	tpage[2] = LoadTPage(mat2+0x80, 0, 0, 704,  0, 256,256);
	tpage[3] = LoadTPage(mat3+0x80, 0, 0, 704,256, 256,256);

	/* load 4 texture CLUTS */
	clut[0] = LoadClut(mat0, 0,500);
	clut[1] = LoadClut(mat1, 0,501);
	clut[2] = LoadClut(mat2, 0,502);
	clut[3] = LoadClut(mat3, 0,503);
}

