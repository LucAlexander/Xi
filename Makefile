CC=gcc
CCWIN=x86_64-w64-mingw32-gcc
WINFLAGS=-lmingw32
CFLAGS=-lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lm -ftrapv -O3
CDEBUGFLAGS=-Wall -g
WINLIBS=-ISDL2/include/ -LSDL2/lib/
OUT=xiprog
BUILDWIN=build-win/
BUILDLINUX=build-linux/

CCWASM=emcc
PORT_WASM=7000
WASMLIBS=--bind -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -lm -O3
BUILDWASM=build-wasm/

PROJECT=xui
USER_FILES=$(wildcard projects/$(PROJECT)/src/*.h) $(wildcard projects/$(PROJECT)/src/*.c)
WASM_USER_FILES = $(wildcard projects/$(PROJECT)/src/*.c)

project:
	cp -r template/ projects/$(PROJECT)

setup-wasm:
	rm -rf $(BUILDWASM)
	rm -rf config/
	mkdir $(BUILDWASM)
	mkdir $(BUILDWASM)./bin/
	cp -r projects/$(PROJECT)/src/config/ .

compile-wasm:
	make setup-wasm
	$(CCWASM) $(wildcard *.c) $(WASM_USER_FILES) $(WASMLIBS) -o $(OUT).html --preload-file projects/$(PROJECT)/ --use-preload-plugins
	mv $(OUT).html $(BUILDWASM)./bin/
	mv $(OUT).js $(BUILDWASM)./bin/
	mv $(OUT).wasm $(BUILDWASM)./bin/
	mv $(OUT).data $(BUILDWASM)./bin/
	cp -r projects/$(PROJECT)/fnt/ $(BUILDWASM)
	cp -r projects/$(PROJECT)/spr/ $(BUILDWASM)
	cp -r projects/$(PROJECT)/snd/ $(BUILDWASM)
	echo "python3 -m http.server 7000" > $(BUILDWASM)./bin/run.sh
	chmod +x $(BUILDWASM)./bin/run.sh

debug-wasm:
	make setup-wasm
	$(CCWASM) $(wildcard *.c) $(WASM_USER_FILES) $(WASMLIBS) $(CDEBUGFLAGS) -o $(OUT)-debug.html --preload-file projects/$(PROJECT)/ --use-preload-plugins
	mv $(OUT)-debug.html $(BUILDWASM)./bin/
	mv $(OUT)-debug.js $(BUILDWASM)./bin/
	mv $(OUT)-debug.wasm $(BUILDWASM)./bin/
	mv $(OUT)-debug.data $(BUILDWASM)./bin/
	cp -r projects/$(PROJECT)/fnt/ $(BUILDWASM)
	cp -r projects/$(PROJECT)/spr/ $(BUILDWASM)
	cp -r projects/$(PROJECT)/snd/ $(BUILDWASM)
	echo "python3 -m http.server 7000" > $(BUILDWASM)./bin/run.sh
	chmod +x $(BUILDWASM)./bin/run.sh

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
	make compile-wasm

debug-all:
	make debug-linux
	make debug-win
	make debug-wasm

clean:
	rm -rf config/
	rm -rf $(BUILDWIN)
	rm -rf $(BUILDLINUX)
	rm -rf $(BUILDWASM)

