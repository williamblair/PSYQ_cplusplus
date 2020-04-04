::batch file for compilation - sometimes psymake can be a bit buggy

:: the psyq make utility
::psymake
ccpsx -O3 -Xo$80010000 main.cpp System.cpp Sprite.cpp Sprite_textured.cpp Texture.cpp ../global_src/MATTEX.C -otuto0.cpe,tuto0.sym,mem.map -llibpress
cpe2x tuto0.cpe

:: converts the generated cpe file to a PSX EXE file, which can be run by an emulator
::cpe2x main.cpe
::copy main.exe cd_root
::pause

::systemcnf main.exe > cd_root/system.cnf
::mkisofs.exe -o main.hsf -V MAIN -sysid PLAYSTATION cd_root
::mkpsxiso main.hsf main.bin C:\psx-sdk/share/licenses/infousa.dat
::pause
