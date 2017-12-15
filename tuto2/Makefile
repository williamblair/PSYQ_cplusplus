# mingw makefile to use with codeblocks,
# psymake doesn't work in 64 bit
# also, GNU make doesn't work
# well with the $ character, even
# escaping it with $$ for
# -Xo$80010000, so comp.bat is used instead

# main compilation:
all:
	comp.bat

# desired output name
# applies to the .cpe and .exe
BIN = tuto2

# psx emulator to run the build with
EMULATOR = C:\psyq\no_psx\NO_PSX.exe

# remove build files
clean:
	rm -rf $(BIN).exe $(BIN).sym mem.map $(BIN).cpe

run:
	$(EMULATOR) ./$(BIN).exe
