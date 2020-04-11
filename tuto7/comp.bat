:: batch file for compilation - psymake doesn't work on 64 bit

:: compile the sources
ccpsx -O3 -Xo$80010000 main.cpp System.cpp Sprite.cpp Sprite_textured.cpp Cube.cpp Pad.cpp Texture.cpp ../global_src/BGTEX.C -otuto7.cpe,tuto7.sym,mem.map -llibpress -llibpad

:: convert into a .exe
cpe2x tuto7.cpe

:: below makes a cd, uncomment if you have psx-sdk set in your
:: environment variables
::copy tuto4.exe cd_root
::systemcnf tuto4.exe > cd_root/system.cnf
::mkisofs.exe -o tuto4.hsf -V TUTO4 -sysid PLAYSTATION cd_root
::mkpsxiso tuto4.hsf tuto4.bin C:\psx-sdk/share/licenses/infousa.dat
