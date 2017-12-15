# MinGW makefile to use with psyq because
# psymake doesn't work in 64 bit; also
# using a batch file for the compile step
# because GNU Make doesn't like
# the $ as a literal character

# last resort:
all:
	comp.bat

# desired output name
# applies to the .cpe and the .exe
BIN = tuto0


# PSX emulator to run the build with
EMULATOR = C:\psyq\no_psx\NO_PSX.exe


clean:
	rm -rf $(BIN).exe $(BIN).sym mem.map $(BIN).cpe

run:
	$(EMULATOR) ./$(BIN).exe
