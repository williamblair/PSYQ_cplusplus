SRC0 =	mattex.c balltex.c traintex.c bgtex.c bgtex8.c \
	balls.c cube.c init.c lscr.c 
OBJ0 =	mattex.obj balltex.obj traintex.obj bgtex.obj bgtex8.obj \
	balls.obj cube.obj init.obj lscr.obj 
SRC1 =	tuto0.c tuto1.c tuto2.c tuto3.c tuto4.c tuto5.c tuto6.c \
	tuto7.c tuto8.c tuto9.c tuto10.c 
OBJ1 =	tuto0.obj tuto1.obj tuto2.obj tuto3.obj tuto4.obj tuto5.obj \
	tuto6.obj tuto7.obj tuto8.obj tuto9.obj tuto10.obj 
SRC2 = 	tuto11.c tuto12.c tuto13.c
OBJ2 = 	tuto11.obj tuto12.obj tuto13.obj

main : tuto0.lib tuto1.lib tuto2.lib
	ccpsx -O3 -Xo$80020000 main.c -ltuto0.lib -ltuto1.lib -ltuto2.lib \
							-omain.cpe,main.sym
for_menu: tuto0.lib tuto1.lib tuto2.lib
	ccpsx -O3 -Xo$80020000 main.c \psx\lib\none3.obj \
		-ltuto0.lib -ltuto1.lib -ltuto2.lib -omain.cpe
	cpe2x main.cpe
	del main.cpe

tuto0.lib:
	ccpsx -O3 -c $(SRC0)
	del tuto0.lib
	psylib /a tuto0.lib $(OBJ0)

tuto1.lib:
	ccpsx -O3 -c $(SRC1)
	del tuto1.lib
	psylib /a tuto1.lib $(OBJ1)

tuto2.lib:
	ccpsx -O3 -c $(SRC2)
	del tuto2.lib
	psylib /a tuto2.lib $(OBJ2)

clean:
	del *.obj
	del *.lib
	del *.cpe
	del *.sym
	del *.exe
