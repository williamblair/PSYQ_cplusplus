/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*				menu
 *
 *             Simple Menu Interface
 *                Simple menu
 *
 *        Copyright  (C)  1997 by Sony Corporation
 *             All rights Reserved
 *
 *    Version  Date      Design
 *   -----------------------------------------
 *   0.10      Jul,15,1997    suzu
  */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

/* [Databale] ---------------------------------------------------------
 *   
 * - Definition of structures determining menu behavior */
#include "menu.h"

/* prototype */
void myPadInit(int mode);
u_long myPadRead(int mode);
void myPadStop(void);
void startSystem(int mode);
void stopSystem(void);
void initMenu(char *title, char **list, char **attr, char **msg, char **help);
int selectMenu(void);

/* [Main] ----------------------------------------------------------
 *   
 * - Starts up sample functions in response to instructions from the menu.
 * - When starting up specific sample functions for examination, specify the functions in
 * the following manner.
 *	
 *   main ()  {
 *        startSystem (0) ;
 *        Balls () ;
 *        stopSystem () ;
 *   } */
#define APD_MODE	0

main()
{
	int id;
	
	/* initialize library*/
	startSystem(APD_MODE);
	
	/* initialize menu*/
	initMenu(_title, _list, _attr, _msg, _help);
	
	/* start up sample program*/
	while ((id = selectMenu()) != -1) 
	       (*_func[id])();

	/* exit library and make inactive*/
	stopSystem();
}

/* [Initialization] ---------------------------------------------
 *   
 * - When using library features, an initialization function must be called
 *   for each function.
 * - Note that calling initialization functions for unused libraries will increase
 *   code size.
 * - The sequence for initialization is arbitrary.  */
void startSystem(int mode)
{
	RECT rect;
	
	ResetGraph(0);		/* initialize Renderer		*/
	InitGeom();		/* initialize Geometry		*/
	DecDCTReset(0);		/* initialize Decompressor	*/
	CdInit();		/* initialize CD-ROM		*/
	
	SetGraphDebug(0);	/* set graphics debug mode */
	
	myPadInit(mode);	/* initialize Controller	*/
		
	/* clear image to reduce display distortion */
	setRECT(&rect, 0, 0, 256, 240);
	ClearImage(&rect, 0, 0, 0);
	SetDispMask(1);
}	

/* [Termination] ----------------------------------------------------
 *
 * - When a program is terminated and a new program is started with Exec(),
 *   the resources used by the libraries must be freed up.
 * - Almost all of the libraries are made inactive with StopCallback().
 *   Controller, memory card, etc. must be handled separately. */
void stopSystem(void)
{
	myPadStop();
	StopCallback();
}

/***************************************************************************/
/*
 *        Controller recorder (prototype)
 *       Records and plays back the contents of the controller
 *--------------------------------------------------------------------------
 * myPadInit   initializes the controller
 *
 * Format      void myPadInit (int mode) 
 *
 * Parameters       mode 0  compatibility with PadInit()
 *             1  record controller contents starting at 0x80010000
 *             2  play back the contents recorded at 0x80010000
 *
 * Description      Records input history of the controller with myPadInit(1) and 
 *        plays back the same input using myPadINit(2).
 *
 * Return value     none
 *   
 * Notes       Memory contents recorded with myPadInit(1) are temporarily saved
 *        to a file so that the same input operations can be repeatedly played back
 *        automatically during program development or debugging.
 *        Programs starting at 0x80010000 must have the start address
 *        offset.
 *
 *--------------------------------------------------------------------------
 * myPadStop   Stop controller
 *
 * Format      void myPadStop (void) 
 *
 * Parameters       none
 *
 * Description      Used in conjunction with myPadInit(). See section on PadStop()
 *
 * Return value          none
 *   
 *
 *--------------------------------------------------------------------------
 * myPadRead   initialize controller
 *
  * Format          void myPadRead (int mode) 
 *
 * Parameter        mode not used (invalid) 
 *
 * Description      Used in conjunction with myPadInit(). See the section on PadInit()
 *
 * Return value     Current controller value
 * */

#define HISTORY_ADDR	0x80010000
typedef struct {
	u_long	data;
	u_long	time;
} HISTORY;
	
static HISTORY *history = (HISTORY *)HISTORY_ADDR;
static int hp;
static int recmode;
static int ctime;

void myPadInit(int mode)
{
	PadInit(0);
	if ((recmode = mode) == 1) {	/* record */
		hp = 0;
		history[hp].data = history[hp].time = 0;
	}
	else if (recmode  == 2) {	/* play */
		hp = 0;
		ctime = history[hp].time;
		load_history();			
	}
	recmode = mode;
}

void myPadStop(void)
{
	PadStop();
	if (recmode == 1) {
		printf("Record done: %d entries used\n",hp);
		save_history();
	}
}

u_long myPadRead(int mode)
{
	static u_long opadd = 0;
	u_long padd = PadRead(0);
	
	switch (recmode) {
	    case 0:
		return(padd);
		
	    case 1:
		if (padd == opadd) 
			history[hp].time++;
		else {
			hp++;
			history[hp].data = padd;
			history[hp].time   = 0;
			opadd = padd;
		}
		return(padd);
		
	    case 2:
		if (ctime > 0) 
			ctime--;
		else 
			ctime = history[++hp].time;
		
		return(history[hp].data);
	}
}

#define FILENAME "main.pad"
save_history()
{
	int fd;
	PCinit();
	if ((fd = PCcreat(FILENAME, 0)) == -1) {
		printf("%s: cannot create\n", FILENAME);	
		return;
	}
	PCwrite(fd, HISTORY_ADDR, 64*1024);
	PCclose(fd);
}
load_history()
{
	int fd;
	PCinit();
	if ((fd = PCopen(FILENAME, 0, 0)) == -1) {
		printf("%s: cannot open\n", FILENAME);	
		return;
	}
	PCread(fd, HISTORY_ADDR, 64*1024);
	PCclose(fd);
}

/***************************************************************************/
/*
 *             Basic menu system
 *                (Prototype)
 *--------------------------------------------------------------------------
 * initMenu    initialize menu
 *
  * Format          void initMenu (char *title,
 *             char **list, char **attr, char **msg, char **help) 
 *
 * Parameters  title     title string
 *        list menu entry strings
 *        attr attribute string indicating cursor position
 *             the cursor follows the non-whitespace characters in attr.
 *             a cursor with the length specified there is displayed
 *        msg  user reserved string. displayed at the bottom of the menu
 *        help help message
 *
 * Description initializes the menu system
 *
 * Return value     none
 *   
 * Notes       There are restrictions on the format that can be used in list. 
 *            columns 0-7: sample name
 *            columns 8-11: DEMO sample startup button
 *            columns 14-: summary of sample
 *   There are restrictions on the strings that can be used for attr. See menu.h.
 *
 *--------------------------------------------------------------------------*/
/* define for AutoPad */
#define PadRead(x)	myPadRead(x)

/*
 * Structure
 */
/* TILE with matte */
typedef struct {
	DR_MODE	mode;
	TILE	tile;
	LINE_F3	line0;
	LINE_F3	line1;
} RTILE;
	
/* menu status */
typedef struct {
	int		mode;		/* attribute */
	int		x0, y0, w, h;	/* widow size */
	char		*title;		/* title string */
	char		**list;		/* menu list */
	char		**attr;		/* menu map */
	char		*msg;		/* usr message */
	SPRT_8		*sprt;		/* primitive buffer   */
	int		cx, cy;		/* cursor position */
	u_long		ot[4];		/* OT */
	RTILE		bg;		/* background */
	RTILE		tbar;		/* title bar */
	RTILE		cursor;		/* cursor */
	DR_MODE		f_drmode;	/* font draw mode */
	SPRT_8		*sp;		/* sprite pointer */
	u_long		opadd;		/* old pad data */
} MENU;

/*
 * Prototypes
 */
void MenuFntLoad(int tx, int ty);
void MenuInit(MENU *menu, int r, int g, int b);
void MenuUpdate(MENU *menu, u_long padd);
void MenuFlush(MENU *menu, u_long padd, int isadj);

/************************************************************************/
/*									*/
/*			Convenient API					*/
/*									*/
/************************************************************************/

static char	_msgbuf0[256], _msgbuf1[256];
static SPRT_8	_sprtbuf0[1024], _sprtbuf1[1024];

/* main menu */
static MENU menu0 = {
	1,				/* flat background */
	64, 32, 0, 0,			/* position and size */
	0,				/* menu title */
	0,				/* menu strings */
	0,				/* menu attribute */
	_msgbuf0,
	_sprtbuf0,			/* primitive buffer */
};

/* help menu */
static MENU menu1 = {
	1,				/* flat background */
	160, 40, 128, 0,		/* position and size */
	"Help",				/* menu title */
	0,				/* menu strings */
	0,				/* menu attribute */
	_msgbuf1,
	_sprtbuf1,			/* primitive buffer */
};

static DRAWENV	draw[2];
static DISPENV	disp[2];
static char	**msg;
static char	**help;

/*
 * initialize menu system
 */
void initMenu(char *title, char **list, char **attr, char **_msg, char **_help)
{
	menu0.title = title;
	menu0.list  = list;
	menu0.attr  = attr;
	
	msg  = _msg;
	help = _help;
	
	SetDefDrawEnv(&draw[0], 0,   0, 512, 240);
	SetDefDispEnv(&disp[0], 0, 240, 512, 240);
	SetDefDrawEnv(&draw[1], 0, 240, 512, 240);
	SetDefDispEnv(&disp[1], 0,   0, 512, 240);
	
	draw[0].isbg = draw[1].isbg = 1;
	setRGB0(&draw[0], 60, 120, 120);
	setRGB0(&draw[1], 60, 120, 120);
	
	MenuFntLoad(960, 256);	
	MenuInit(&menu0,  64, 64, 128);
	MenuInit(&menu1, 128, 64, 128);
}

/*
 * update menu system
 */
int selectMenu(void)
{
	u_long padd;
	int	id = 0;
	
	MenuFntLoad(960, 256);	
	while (PadRead(0));
	while ((padd = PadRead(0)) != PADselect) {
		
		strcpy(menu0.msg, msg[menu0.cy]);
		MenuFlush(&menu0, padd, 1);
		
		if (padd&PADRright && menu0.cx == 14) {
			int i = 0, j = 0;
			
			while (j != menu0.cy)
				if (help[i++] == 0)
					j++;
			
			menu1.list = help+i;
			MenuFlush(&menu1, padd, 1);
		}
		
		if (padd&PADRright && menu0.cx == 9) {
			while (PadRead(0));
			return(menu0.cy);
		}
		
		VSync(0);
		PutDispEnv(&disp[id]);
		PutDrawEnv(&draw[id]);
		id = (id+1)&0x01;

	}
	return(-1);
}


/************************************************************************/
/*									*/
/*			Basic Library					*/
/*									*/
/************************************************************************/

static int strtosprt(MENU *menu, char *s, int x, int y);
static int cursor_length(MENU *menu);
static int cursor_home(MENU *menu);
static int cursor_up(MENU *menu);
static int cursor_down(MENU *menu);
static int cursor_next(MENU *menu);
static int cursor_prev(MENU *menu);
	

#include "fonttex.c"

static u_short	fclut, ftpage;

void MenuFntLoad(int tx, int ty)
{
	fclut   = LoadClut2(font, tx, ty+128);
	ftpage  = LoadTPage(font+0x80, 0, 0, tx, ty, 128, 32);
}

static setRTile(RTILE *rtile, int abe, int abr, int r, int g, int b)
{
	int r2 = (r*2>255)? 255: r*2;
	int g2 = (g*2>255)? 255: g*2;
	int b2 = (b*2>255)? 255: b*2;

	/* window BG tile */
	SetTile(&rtile->tile);
	SetDrawMode(&rtile->mode, 0, 0, GetTPage(0, abr, 0, 0), 0);
	SetSemiTrans(&rtile->tile, abe);
	MargePrim(&rtile->mode, &rtile->tile);
	
	/* window edge */
	SetLineF3(&rtile->line0);
	SetSemiTrans(&rtile->line0, abe);
	SetLineF3(&rtile->line1);
	SetSemiTrans(&rtile->line1, abe);
	MargePrim(&rtile->line0, &rtile->line1);

	/* linkage */
	CatPrim(&rtile->mode, &rtile->line0);

	/* set color */
	setRGB0(&rtile->tile,  r,   g,   b);	/* window */
	setRGB0(&rtile->line0, r2,  g2,  b2);	/* upper/left  edge */
	setRGB0(&rtile->line1, r/2, g/2, b/2);	/* lower/right edge */
}

static setRTileXYWH(RTILE *rtile, int x, int y, int w, int h)
{
	setXY0(&rtile->tile, x, y);
	setWH( &rtile->tile, w, h);
	setXY3(&rtile->line0, x, y+h, x,   y,   x+w, y);
	setXY3(&rtile->line1, x, y+h, x+w, y+h, x+w, y);
}
	
static addRTile(u_long *ot, RTILE *rtile)
{
	AddPrims(ot, &rtile->mode, &rtile->line0);
}
	
void MenuInit(MENU *menu, int r, int g, int b)
{
	/* background */
	setRTile(&menu->bg, 0, 0, r, g, b);
	
	/* title bar */
	setRTile(&menu->tbar, 1, 0, 64, 64, 64);

	/* cursor */
	setRTile(&menu->cursor, 1, 1, 64, 64, 64);
	
	/* font texture */
	SetDrawMode(&menu->f_drmode, 0, 0, ftpage, 0);
	
	/* reset */
	if (menu->msg)		menu->msg[0] = 0;
	if (menu->attr)		cursor_home(menu);
	menu->sp = menu->sprt;
}

void MenuFlush(MENU *menu, u_long padd, int isadj)
{
	char	**s;
	int	x = 0, y = 0,len;
	
	/* padd update */
	if (menu->opadd == 0 && menu->attr) {
		if (padd&PADLright)		cursor_next(menu);
		if (padd&PADLleft)		cursor_prev(menu);
		if (padd&PADLup)		cursor_up(menu);
		if (padd&PADLdown)		cursor_down(menu);
	}
	menu->opadd = padd;
	
	/* adjust menu size */
	if (isadj) {
		menu->w = menu->h = 0;
	
		if (menu->title)
			menu->h += 16;
		if (menu->list) {
			for (y = 0; menu->list[y]; y++) 
				if (x < (len = strlen(menu->list[y])))
					x = len;
			menu->w  = x*8+2;
			menu->h += y*8;
		}
		if (menu->msg) { 
			if (menu->w < (len = strlen(menu->msg)*8+2))
				menu->w = len;
			menu->h += 12;
		}
	}
	
	/* linkage */
	ClearOTag(menu->ot, 4);
	
	/* set Sprte mode */
	AddPrim(menu->ot+2,  &menu->f_drmode);
	
	/* set BG */
	if (menu->mode) {
		addRTile(menu->ot, &menu->bg);
		setRTileXYWH(&menu->bg,
			     menu->x0-2, menu->y0-2, menu->w+2, menu->h+2);
	}

	/* set TItle */
	y = 0;
	if (menu->title) {
		addRTile(menu->ot+1, &menu->tbar);
		y = strtosprt(menu, menu->title, 0, y);
		setRTileXYWH(&menu->tbar, menu->x0, menu->y0, menu->w, y+8);
		y += 16;
	}
	
	/* set cursor */
	if (menu->attr) {
		addRTile(menu->ot+3, &menu->cursor);
		setRTileXYWH(&menu->cursor,
			     menu->x0+menu->cx*8, menu->y0+menu->cy*8+y,
			     cursor_length(menu), 8);
	}
	
	/* draw text on window */
	if (menu->list) {
		for (s = menu->list; *s; s++) {
			y = strtosprt(menu, *s, 0, y);
			y += 8;
		}
		y += 4;
	}

	/* draw use message */
	if (menu->msg) 
		strtosprt(menu, menu->msg, 0, y);
	
	/* draw */
	DrawOTag((u_long *)menu->ot);

	/* reset */
	menu->sp = menu->sprt;
}

static int strtosprt(MENU *menu, char *s, int x, int y)
{
	SPRT_8	*sp;
	u_char	u, v;
	int	code, isret;
	int	r = 128, g = 128, b = 128;
	
	sp = menu->sp;

	while (*s) {
		isret = 0;
		switch (*s) {
		    case '~':
			switch (*++s) {
			    case 'c':
				r = (*++s-'0')*16;
				g = (*++s-'0')*16;
				b = (*++s-'0')*16;
				break;
			}
			break;

		    case '\n':
			isret = 1;
			break;
			
		    case '\t':
			if ((x += 8*4) >= menu->w) isret = 1;
			break;
			
		    case ' ':
			if ((x += 8) >= menu->w) isret = 1;
			break;
			
		    default:
			if (*s >= 'a' && *s <= 'z') 
				code = *s - 'a' + 33;
			else
				code = *s - '!' + 1;
			
			u = (code%16)*8;
			v = (code/16)*8;
			
			SetSprt8(sp);
			setUV0(sp, u, v);
			setXY0(sp, x+menu->x0, y+menu->y0);
			setRGB0(sp, r, g, b);
			sp->clut = fclut;
			AddPrim(&menu->f_drmode, sp++);
			
			if ((x += 8) >= menu->w) isret = 1;
			break;
		}
		if (isret) {
			x = 0;
			if ((y += 8) >= menu->h)
				break;
		}
		s++;
	}
	menu->sp = sp;
	return(y);
}

static int cursor_length(MENU *menu)
{
	int w = menu->attr[menu->cy][menu->cx];
	if (w == ' ')
		return(0);
	else
		return((w-'0')*8);
}

static int cursor_home(MENU *menu)
{
	int	x, y;
	for (y = 0; menu->attr[y]; y++) 
		for (x = 0; menu->attr[y][x]; x++)
			if (menu->attr[y][x] != ' ') {
				menu->cx = x;
				menu->cy = y;
				return(0);
			}
	printf("Menu: home position not found\n");
	return(-1);
}

static int cursor_next(MENU *menu)
{
	int	x = menu->cx+1;
	int	y = menu->cy;
	
	while (menu->attr[y]) {
		while (menu->attr[y][x]) {
			if (menu->attr[y][x] != ' ') {
				menu->cx = x;
				menu->cy = y;
				return(0);
			}
			x++;
		}
		x = 0, y++;
	}
	return(cursor_home(menu));
}

static int cursor_prev(MENU *menu)
{
	if (menu->attr) {
		int	x = menu->cx-1;
		int	y = menu->cy;
	
		while (1) {
			while (x >= 0) {
				if (menu->attr[y][x] != ' ') {
					menu->cx = x;
					menu->cy = y;
					return(0);
				}
				x--;
			}
			if (--y < 0) {
				for (y = 0; menu->attr[y]; y++);
				y--;
			}
			x = strlen(menu->attr[y])-1;
		}
	}
	return(0);
}
	
static int cursor_down(MENU *menu)
{
	if (menu->attr[++menu->cy] == 0)	
		menu->cy = 0;
	
	if (menu->attr[menu->cy][menu->cx] != ' ') 
		return(0);
	cursor_next(menu);
}
	

static int cursor_up(MENU *menu)
{
	if (--menu->cy < 0) {
		int y;
		for (y = 0; menu->attr[y]; y++);
		menu->cy = y-1;
	}
	if (menu->attr[menu->cy][menu->cx] != ' ') 
		return(0);
	
	cursor_next(menu);
}


