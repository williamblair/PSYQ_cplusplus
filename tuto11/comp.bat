:: batch file for compilation - psymake doesn't work on 64 bit

:: compile the sources
ccpsx -O3 -Xo$80010000 main.cpp System.cpp Sprite.cpp Sprite_textured.cpp Cube.cpp Pad.cpp Texture.cpp ../global_src/BGTEX.C -otuto11.cpe,tuto11.sym,mem.map -llibpress -llibpad

:: convert into a .exe
cpe2x tuto11.cpe

:: below makes a cd, uncomment if you have psx-sdk set in your
:: environment variables
copy tuto11.exe cd_root
systemcnf tuto11.exe > cd_root/system.cnf
mkisofs.exe -o tuto11.hsf -V TUTO11 -sysid PLAYSTATION cd_root
mkpsxiso tuto11.hsf tuto11.bin D:/psx-sdk/share/licenses/infousa.dat
