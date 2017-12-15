:: batch file for compilation - psymake doesn't work on 64 bit

:: compile the sources
ccpsx -O3 -Xo$80010000 main.cpp tuto4.cpp -otuto4.cpe,tuto4.sym,mem.map -llibpress

:: convert into a .exe
cpe2x tuto4.cpe

:: below makes a cd, uncomment if you have psx-sdk set in your
:: environment variables
::copy tuto4.exe cd_root
::systemcnf tuto4.exe > cd_root/system.cnf
::mkisofs.exe -o tuto4.hsf -V TUTO4 -sysid PLAYSTATION cd_root
::mkpsxiso tuto4.hsf tuto4.bin C:\psx-sdk/share/licenses/infousa.dat
