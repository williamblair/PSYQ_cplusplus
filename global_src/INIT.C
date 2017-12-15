/* $PSLibId: Run-time Library Release 4.4$ */
/*				init
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/*		      Initialize graphic environment */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* int x, y;	/* GTE offset		*/
/* int z;	/* distance to the screen	*/
/* int level;	/* debug level		*/

extern char	*progname;

void init_system(int x, int y, int z, int level)
{
	/* reset graphic subsystem*/
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));
	
	/* set debug mode (0:off, 1:monitor, 2:dump)  */
	SetGraphDebug(level);	

	/* initialize geometry subsystem*/
	InitGeom();			
	
	/* set geometry origin as (160, 120)*/
	SetGeomOffset(x, y);	
	
	/* distance to veiwing-screen*/
	SetGeomScreen(z);		
}
