:: batch file for compilation - psymake doesn't work on 64 bit

:: compile the sources
ccpsx -O3 -Xo$80010000 main.cpp System.cpp Sprite.cpp Sprite_textured.cpp Texture.cpp -otuto2.cpe,tuto2.sym,mem.map -llibpress

:: convert into a .exe
cpe2x tuto2.cpe

:: below makes a cd, uncomment if you have psx-sdk set in your
:: environment variables
::copy tuto2.exe cd_root\
::systemcnf tuto2.exe > cd_root/system.cnf
::mkisofs.exe -o tuto2.hsf -V TUTO2 -sysid PLAYSTATION cd_root
::mkpsxiso tuto2.hsf tuto2.bin C:\psx-sdk/share/licenses/infousa.dat
