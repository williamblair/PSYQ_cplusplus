::batch file for compilation - psymake doesn't work on 64bit

:: the psyq make utility
::psymake
ccpsx -O3 -Xo$80010000 main.cpp System.cpp Sprite.cpp Sprite_textured.cpp Texture.cpp ../global_src/BALLTEX.C -otuto1.cpe,tuto1.sym,mem.map -llibpress
::pause

:: converts the generated cpe file to a PSX EXE file, which can be run by an emulator
cpe2x tuto1.cpe
::copy tuto1.exe cd_root
::pause

::systemcnf tuto1.exe > cd_root/system.cnf
::mkisofs.exe -o tuto1.hsf -V TUTO1 -sysid PLAYSTATION cd_root
::mkpsxiso main.hsf main.bin C:\psx-sdk/share/licenses/infousa.dat
::pause
