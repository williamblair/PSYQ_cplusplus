:: batch file for compilation - psymake doesn't work on 64 bit

:: compile the sources
ccpsx -O3 -Xo$80010000 main.cpp System.cpp Sprite.cpp Sprite_textured.cpp Cube.cpp Pad.cpp Texture.cpp ../global_src/BALLS.C ../global_src/BALLTEX.C -otuto13.cpe,tuto13.sym,mem.map -llibpress -llibpad

:: convert into a .exe
cpe2x tuto13.cpe

:: below makes a cd, uncomment if you have psx-sdk set in your
:: environment variables
copy tuto13.exe cd_root
systemcnf tuto13.exe > cd_root/system.cnf
mkisofs.exe -o tuto13.hsf -V TUTO11 -sysid PLAYSTATION cd_root
mkpsxiso tuto13.hsf tuto13.bin D:/psx-sdk/share/licenses/infousa.dat
