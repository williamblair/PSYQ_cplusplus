:: batch file for compilation - psymake doesn't work on 64 bit

:: compile the sources
ccpsx -O3 -Xo$80010000 main.cpp System.cpp Sprite.cpp Sprite_textured.cpp Cube.cpp Pad.cpp Texture.cpp ../global_src/BALLTEX.C -otuto12.cpe,tuto12.sym,mem.map -llibpress -llibpad

:: convert into a .exe
cpe2x tuto12.cpe

:: below makes a cd, uncomment if you have psx-sdk set in your
:: environment variables
copy tuto12.exe cd_root
systemcnf tuto12.exe > cd_root/system.cnf
mkisofs.exe -o tuto12.hsf -V TUTO12 -sysid PLAYSTATION cd_root
mkpsxiso tuto12.hsf tuto12.bin D:/psx-sdk/share/licenses/infousa.dat
