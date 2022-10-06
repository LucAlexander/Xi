CC=gcc
CCWIN=x86_64-w64-mingw32-gcc
WINFLAGS=-lmingw32
CFLAGS=-lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lm -ftrapv -O3
CDEBUGFLAGS=-Wall -g
WINLIBS=-ISDL2/include/ -LSDL2/lib/
OUT=xiprog
BUILDWIN=build-win/
BUILDLINUX=build-linux/

PROJECT=test
USER_FILES=$(wildcard projects/$(PROJECT)/src/*.h) $(wildcard projects/$(PROJECT)/src/*.c)

project:
	cp -r template/ projects/$(PROJECT)

setup-linux:
	rm -rf $(BUILDLINUX)
	rm -rf config/
	mkdir $(BUILDLINUX)
	mkdir $(BUILDLINUX)./bin/
	cp -r projects/${PROJECT}/src/config/ .

compile-linux:
	make setup-linux
	$(CC) $(wildcard *.h) $(wildcard *.c) $(USER_FILES) $(CFLAGS) -o $(OUT)
	mv $(OUT) $(BUILDLINUX)./bin/
	cp -r projects/$(PROJECT)/fnt/ $(BUILDLINUX)
	cp -r projects/$(PROJECT)/spr/ $(BUILDLINUX)
	cp -r projects/$(PROJECT)/snd/ $(BUILDLINUX)

debug-linux:
	make setup-linux
	$(CC) $(wildcard *.h) $(wildcard *.c) $(USER_FILES) $(CFLAGS) $(CDEBUGFLAGS) -o $(OUT)-debug
	mv $(OUT)-debug $(BUILDLINUX)./bin/
	cp -r projects/$(PROJECT)/fnt/ $(BUILDLINUX)
	cp -r projects/$(PROJECT)/spr/ $(BUILDLINUX)
	cp -r projects/$(PROJECT)/snd/ $(BUILDLINUX)

setup-win:
	rm -rf $(BUILDWIN)
	rm -rf config/
	mkdir $(BUILDWIN)
	mkdir $(BUILDWIN)./bin/
	cp -r projects/$(PROJECT)/src/config/ .

compile-win:
	make setup-win
	$(CCWIN) $(wildcard *.h) $(wildcard *.c) $(USER_FILES) $(WINLIBS) $(WINFLAGS) $(CFLAGS) -o $(OUT).exe
	mv $(OUT).exe $(BUILDWIN)./bin/
	cp $(wildcard *.dll) $(BUILDWIN)./bin/
	cp -r projects/$(PROJECT)/fnt/ $(BUILDWIN)
	cp -r projects/$(PROJECT)/spr/ $(BUILDWIN)
	cp -r projects/$(PROJECT)/snd/ $(BUILDWIN)

debug-win:
	make setup-win
	$(CCWIN) $(wildcard *.h) $(wildcard *.c) $(USER_FILES) $(WINLIBS) $(WINFLAGS) $(CFLAGS) $(CDEBUGFLAGS) -o $(OUT)-debug.exe
	mv $(OUT)-debug.exe $(BUILDWIN)./bin/
	cp $(wildcard *.dll) $(BUILDWIN)./bin/
	cp -r projects/$(PROJECT)/fnt/ $(BUILDWIN)
	cp -r projects/$(PROJECT)/spr/ $(BUILDWIN)
	cp -r projects/$(PROJECT)/snd/ $(BUILDWIN)

compile-all:
	make compile-linux
	make compile-win

debug-all:
	make debug-linux
	make debug-win

clean:
	rm -rf config/
	rm -rf $(BUILDWIN)
	rm -rf $(BUILDLINUX)

