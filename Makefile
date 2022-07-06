CC=gcc
CCWIN=x86_64-w64-mingw32-gcc
WINFLAGS=-lmingw32
CFLAGS=-lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lm
CDEBUGFLAGS=-Wall -g
WINLIBS=-ISDL2/include/ -LSDL2/lib/
ENTRYPOINT=main.c
FILES=cflags.h cflags.c systems.h systems.c asciiEsc.h entities.h entities.c hashMap.h hashMap.c vector.h vector.c memarena.h memarena.c bitmasks.h bitmasks.c graphicsutils.c graphicsutils.h inpututils.c inpututils.h mathutils.c mathutils.h sdlfileutils.c sdlfileutils.h
OUT=xiprog
BUILDWIN=build-win/
BUILDLINUX=build-linux/

compile-linux:
	rm -rf $(BUILDLINUX)
	mkdir $(BUILDLINUX)
	$(CC) $(ENTRYPOINT) $(FILES) $(CFLAGS) -o $(OUT)
	mv $(OUT) $(BUILDLINUX)

debug-linux:
	rm -rf $(BUILDLINUX)
	mkdir $(BUILDLINUX)
	$(CC) $(ENTRYPOINT) $(FILES) $(CFLAGS) $(CDEBUGFLAGS) -o $(OUT)-debug
	mv $(OUT)-debug $(BUILDLINUX)

compile-win:
	rm -rf $(BUILDWIN)
	mkdir $(BUILDWIN)
	$(CCWIN) $(ENTRYPOINT) $(FILES) $(WINLIBS) $(WINFLAGS) $(CFLAGS) -o $(OUT).exe
	mv $(OUT).exe $(BUILDWIN)
	cp SDL2_image.dll $(BUILDWIN)
	cp SDL2.dll $(BUILDWIN)
	cp SDL2_ttf.dll $(BUILDWIN)

debug-win:
	rm -rf $(BUILDWIN)
	mkdir $(BUILDWIN)
	$(CCWIN) $(ENTRYPOINT) $(FILES) $(WINLIBS) $(WINFLAGS) $(CFLAGS) $(CDEBUGFLAGS) -o $(OUT)-debug.exe
	mv $(OUT)-debug.exe $(BUILDWIN)
	cp SDL2_image.dll $(BUILDWIN)
	cp SDL2.dll $(BUILDWIN)
	cp SDL2_ttf.dll $(BUILDWIN)

compile-all:
	make compile-linux
	make compile-win

debug-all:
	make debug-linux
	make debug-win

clean:
	rm -rf $(BUILDWIN)
	rm -rf $(BUILDLINUX)



