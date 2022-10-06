# Ξ
(xi) is a software development tool which utilizes an architecture optimized for high locality of reference and high cache friendliness, while also remaining lightweight and maleable. Most software today is much slower than it needs to be, most people just dont know it.

Ξ uses an Entity Component System (ECS) to achieve its speed. The proof of concept precursor to this utility was [Locality](https://github.com/LucAlexander/Locality), which was developed as a semester project for university, this precursor however used a slightly different architecture and ECS, which while slower, was more memory efficient. This version is faster, lighter, and uses an arena allocator to deal with memory management.

## Current Development
Until now i've been using SDL2 to do all media features of the software engine. But after optimizing the internal architecture, SDL2's rendering specification has become the only bottleneck in an otherwise very fast software solution. Im slowly trying to replace SDL2 with OpenGl, which should be able to hangle rendering and user input, however audio capabilities will have to come later. Aside from being much faster, OpenGL also has shader support, which im excited for. In the meantime however, Open Gl will have to wait, as there are bigger things in life than a high standard of software quality.

## Documentation

**Sorry** im kinda bad at writing documentation, its a pain that no programmer enjoys, but as a principle of discipline I try to make all my code as readable as possible without sacrificing any efficiency or simplicity. Go read through my code to gain a better understanding of this software system.

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
```c
void project_structs_init(xi_utils* xi){	
	project_structs* project = malloc(sizeof(project_structs));
	xi->project = project;
	// initialize instances of structures here
}
```
is where the struct defined in `project.h` is instantiated and registered by the engine. This is where you can initialize the data of this struct for future use.

The next function:
```c
void xisetup(xi_utils* xi){
	// register your systems here
}

```
is used to setup any custom systems for you project, and register them with the engine. Many systems and components are built in to the engine already.

The final function:
```c
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

### Entities
Entities conceptually are just an id, which acts as an index in various blocks of memory containing components. Entities function as sets which can contain components as items. Entities also havev a flag system.

```c
typedef enum FLAGS{
	ENTITY_DEACTIVATED=1,
	ENTITY_SOLID=2
}FLAGS;
```

### Built in Components
`POSITION_C` is a label describing a `v2` which holds coordinate data in cartesian space.
`FORCES_C` is a label describing a `v2` which holds horizontal and verical forces being applied to a position.

`BLITABLE_C` is a label describing a `Blitable` image. 
```c
typedef enum BLITABLE_FLAGS{
	BLITABLE_VISIBLE=1,
	BLITABLE_FLIP_H=2,
	BLITABLE_FLIP_V=4
}BLITABLE_FLAGS;

typedef struct Blitable{
	SDL_Texture* texture;
	SDL_Rect drawBound;
	uint32_t displayW;
	uint32_t displayH;
	int32_t textureW;
	int32_t textureH;
	BLITABLE_FLAGS flags;
	double angle;
	SDL_FPoint center;
}Blitable;

void BlitableInitF(GraphicsHandler* ghandle, Blitable* blit, const char* source, uint32_t w, uint32_t h);
void BlitableInitF_arena(GraphicsHandler* ghandle, Blitable* blit, const char* source, uint32_t w, uint32_t h);
void BlitableInit(Blitable* blit, SDL_Texture* t, uint32_t w, uint32_t h);
void renderBlitable(GraphicsHandler* ghandle, Blitable* blit, float x, float y);
void renderBlitableV2(GraphicsHandler* ghandle, Blitable* blit, struct v2 pos);
void BlitableFree(Blitable* blit);

```
Blitable instances instantiated with `BlitableInitF_arena` do not have to be freed with `BlitableFree`, but any other instance does. 

`BEHAVIOR_C` describes a wrapper around a function pointer of the nature
```c
typedef struct logic_t{
	void (*f)(SYSTEM_ARG_REQUIREMENTS);
}logic_t;
```
`SYSTEM_ARG_REQUIREMENTS` is a standard system header which expands to `struct xi_utils* xi, uint32_t id`.

`xi_utils` is a struct which looks like:
```c
typedef struct xi_utils{
	GraphicsHandler* graphics;
	AudioHandler* audio;
	input* user_input;
	entity_data* ecs;
	spacial_quadtree_node_t* colliders;
	struct project_structs* project;
	uint32_t ticks;
	uint8_t* debug;
}xi_utils;
```
`project` is where you can access you project structures. Keep note of the rest, if you see a common datatype in a function you are seeking to use, rest assured that you can acccess the correct instance of that data type through `xi`.

`REPEATER_C` describes a behavior component which repeats rythmically. 
```c
typedef struct repeater_t{
	void (*f)(SYSTEM_ARG_REQUIREMENTS);
	uint32_t ticks;
	uint32_t trigger_time;
	uint32_t trigger_count;
	uint8_t destroy_after;
}repeater_t;

void repeater_t_init(repeater_t* wrapper, void f(SYSTEM_ARG_REQUIREMENTS), uint32_t interval_time, uint32_t count, uint8_t destroy);
```

`CLICKABLE_C` executes some given function pointer after some boundry anchored at a position is clicked.
```c
void clickable_t_init(clickable_t* clicker, void f(SYSTEM_ARG_REQUIREMENTS), int32_t recharge_time, uint32_t w, uint32_t h);

typedef struct text_node_t{
	const char* text;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
}text_node_t;
```

`ANIMATOR_C` describes a component which chooses which frames in a Blitable spritesheet to display at which times. 

```c

typedef struct animator_t{
	const char* name;
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t bounds_x;
	uint32_t bounds_y;
	int32_t index;
	/* active
	 * loop
	 */
	uint8_t flags;
	uint32_t frame_time;
	uint32_t frame_time_counter;
}animator_t;

void animator_t_init(animator_t* animator);
void animator_set_animation(GraphicsHandler* ghandle, animator_t* animator, const char* name, Blitable* sprite);
void animator_set_loop(animator_t* animator, uint8_t loop);
void animator_set_active(animator_t* animator, uint8_t active);
uint8_t animator_get_loop(animator_t* animator);
uint8_t animator_get_active(animator_t* animator);
void animator_set_frame_time(animator_t* animator, uint32_t ft);
uint32_t animator_get_frame_time(animator_t* animator);
```

`TEXT_C` describes a `text_node_t` which holds data about text being rendered to the screen at some position.
```c
typedef struct text_node_t{
	const char* text;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
}text_node_t;

void text_node_t_init(text_node_t* node, const char* text, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void text_node_set_text(text_node_t* node, const char* text);
void text_node_set_color(text_node_t* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
```

`SINGLE_RUN_CONTROLLER_C` describes a component which should only be part of a single entity within a software system. This entity is where all code will run that should only happen once per update, regardless of the number of entities which have other components sought after by a single runtime system.

`COLLIDER_C` describes a `v4` boundary which will collide with collision masks.
**NOTE** built-in collisions are a WIP, but you can simulate your own primitive collisions quite easily, the point of having built in ones is to allow a large number of collidable entities without a significant slowdown, but a few collisions with a bruteforce method will be completely fine. 

### Built in Systems
The following systems exist implicitly, and will act on your entities if the component requirements are met.
`forces_s`: forces components are applies to position components within the same entity.

`blitable_s` Blitable components are drawn at position components.
`text_s` text components are drawn at position components.

`behavior_s` executes logic function pointers.
`repeater_s` execute repeater function pointers at rythmic intervals.
`clickable_s` checks for clicks within the bounded region, and executes the function pointer contained if so.
`draw_clickable_s` draws the clickable region if internal debug symbols are turned on. **NOTE** that the make targets which do debug builds do not modify this variable, `xi->debug` can however be toggled anywhere.

`animate_s` applies animator mutations upon the visible region of a Blitable component.

`draw_entity_colliders_s` draw WIP colliders for entities with a collider and a position.
`draw_world_colliders_s` draw WIP environment colliders.

`solid_collision_s` processes WIP entity to environemnt collisions for entities with a collider and a posiiton.

### Making new Systems
Systems can be created with `system_t system_init(void f(SYSTEM_ARG_REQUIREMENTS), uint32_t n, ...);` which takes a function pointer to the system and a variadic list of component label ID's. This system can then be registered with `void system_add(program_state* state, struct system_t, PROGRAM_STATE);` which takes the xi system state, and a program state.
```c
typedef enum PROGRAM_STATE{
	XI_STATE_UPDATE_PRE,
	XI_STATE_UPDATE,
	XI_STATE_UPDATE_POST,
	XI_STATE_RENDER,
	...
}PROGRAM_STATE;
...
```
Program states occur in this order. 

Systems can have a variety of flags applied to them which interact with entity flags.
The equation for whether a system runs based on the the magnet (M), requisite (R), filter (F) flags of a system and the flag property of an entity (T) = T&M | T&~F | ~R&~T.

To write a system, use the macro:
```c
SYSTEM(name){
	...
}
```
The function pointer would then point to a function called `name`.

### Creating Entities
Check out the demo project's main project file to see how entities and components can be used to manifest behavior out of a simple system of code.

### Sound
Sound is separated by music and sound effects.
Sounds can be added with:
```c
void add_sound(AudioHandler* ah, const char* source, const char* name);
void add_music(AudioHandler* ah, const char* source, const char* name);
```

and played with:
```c
void play_sound(AudioHandler* ah, const char* name);
void play_music(AudioHandler* ah, const char* name);
```

Again, go read the actual code for a more comprehensive look at how to use it, I didnt cover some of the simpler systems because the files which contain them are so self contained and simple that you can just use the actual file as documentation. My code is readable enough to learn from. For example just go look at `input.h` and `input.c`.
