# Ξ
(xi) is a software development tool which utilizes an architecture optimized for high locality of reference and high cache friendliness, while also remaining lightweight and maleable. Most software today is much slower than it needs to be, most people just dont know it.

Ξ uses an Entity Component System (ECS) to achieve its speed. The proof of concept precursor to this utility was [Locality](https://github.com/LucAlexander/Locality), which was developed as a semester project for university, this precursor however used a slightly different architecture and ECS, which while slower, was more memory efficient. This version is faster, lighter, and uses an arena allocator to deal with memory management.

## Current Development
Until now i've been using SDL2 to do all media features of the software engine. But after optimizing the internal architecture, SDL2's rendering specification has become the only bottleneck in an otherwise very fast software solution. Im slowly trying to replace SDL2 with OpenGl, which should be able to hangle rendering and user input, however audio capabilities will have to come later. Aside from being much faster, OpenGL also has shader support, which im excited for. In the meantime however, Open Gl will have to wait, as there are bigger things in life than a high standard of software quality.

## Documentation

### Environment
In the Makefile, find `PROJECT=...`, and change the varible assignment to whichever project you are working on at any given moment. If I wanted to work on my project called "test" for example, I would change it to `PROJECT=test`

To create a new project, first change this variable assignment to your new projects name, and then run `make project`. Projects will appear in the `projects/` directory.


**NOTE** The development environment is currently setup for linux with cross compilation to windows possible using `x86_64-w64-mingw32-gcc`, however to develop on windows, switch the variable `CCWIN` to the correct mingw32 program you have installed and wish to use to compile, such as `gcc` or `g++`.

For woking on linux as intended however, use:
`make debug-linux` for a debug build compiled for linux,
`make debug-win` for a cross compiled debug build for windows,
`make compile-linux` for a release build compiled for linux,
`make compile-win` for a cross compiled release build for windows.

`make debug-all` and `make compile-all` will create debug and compile builds for all target systems.

linux binaries will be located in `build-linux`,
windows binaries will be locatedin `build-win`.

SDL2 has been included in this repository, so you hopefully dont have to install anything extra unless you're cross compiling to windows from linux, in which you will need to install sdl2 for linux, and install `x86_64-w64-mingw32-gcc`.

Use `make clean` to remove compiled binaries, as well as generated intermediary compilation files.

### Navigating a Project
Projects contain four key directories, lets say we have a project called psi which exists as a standard project in the `projects` directory.
`psi/fnt` will contain font files, such as (`.ttf`) truetype fonts.
`psi/snd` will contain sound files for sound effects and music, such as `.wav` files.
`psi/spr` will contain sprites/images, such as `.png` files.
`psi/src` will contain source code.

`psy/src/xi_engine.h` is a file which includes engine source code into your project
`psy/src/project.h` is a standard header for your project, which by default defines a structure within which you can put anything you want to be accessible within your projects standard systems. 
`psy/src/project.c` is your main project file.

Your project file contains three empty functions by default.
The first:
```
void project_structs_init(xi_utils* xi){	
	project_structs* project = malloc(sizeof(project_structs));
	xi->project = project;
	// initialize instances of structures here
}
```
is where the struct defined in `project.h` is instantiated and registered by the engine. This is where you can initialize the data of this struct for future use.

The next function:
```
void xisetup(xi_utils* xi){
	// register your systems here
}

```
is used to setup any custom systems for you project, and register them with the engine. Many systems and components are built in to the engine already.

The final function:
```
void xistart(xi_utils* xi){
	// setup initial state here
}
```
is used to setup your softwares initial state.

`psy/src/config` is the projects configuration directory.
`psy/src/config/project-config.h` controlls the general parameters by which your software exists, such as the window title, size, the timescale, resource directories, and the initial memory pool size for entity arena allocators.
`psy/src/configproject-components.h` is where you register the existance of custom components needed for your project.

`USER_COMPONENTS` is used in an enumerator to create label ID's for components, these are positionally significant and related to `USER_COMPONENT_SIZES` which delineates the size (in bytes) of each of your custom components. Component labels should be inserted above `COMPONENT_COUNT`.
Sizes should be prefixed with a comma.

### Built in Components
**TODO**

### Making new Components
**TODO**

### Built in Systems
**TODO**

### Making new Systems
**TODO**

### Sound
**TODO**

