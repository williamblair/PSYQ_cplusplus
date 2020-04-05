:: batch file for compilation - psymake doesn't work on 64 bit

:: compile the sources
ccpsx -O3 -Xo$80010000 main.cpp System.cpp Sprite.cpp Sprite_textured.cpp Texture.cpp Cube.cpp Pad.cpp -otuto3.cpe,tuto3.sym,mem.map -llibpress -llibpad

:: convert into a .exe
cpe2x tuto3.cpe

:: below makes a cd, uncomment if you have psx-sdk set in your
:: environment variables
::copy tuto3.exe cd_root
::systemcnf tuto3.exe > cd_root/system.cnf
::mkisofs.exe -o tuto3.hsf -V TUTO3 -sysid PLAYSTATION cd_root
::mkpsxiso tuto3.hsf tuto3.bin C:\psx-sdk/share/licenses/infousa.dat
