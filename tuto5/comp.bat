:: batch file for compilation - psymake doesn't work on 64 bit

set PATH=C:\PSYQ\BIN
set PSYQ_PATH=C:\PSYQ\BIN

:: compile the sources
ccpsx -O3 -Xo$80010000 main.cpp tuto5.cpp -otuto5.cpe,tuto5.sym,mem.map -llibpress

:: convert into a .exe
cpe2x tuto5.cpe

:: below makes a cd, uncomment if you have psx-sdk set in your
:: environment variables
::copy tuto4.exe cd_root
::systemcnf tuto4.exe > cd_root/system.cnf
::mkisofs.exe -o tuto4.hsf -V TUTO4 -sysid PLAYSTATION cd_root
::mkpsxiso tuto4.hsf tuto4.bin C:\psx-sdk/share/licenses/infousa.dat
