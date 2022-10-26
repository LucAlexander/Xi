#include <stdio.h>

#include "project.h"
#include "xi_engine.h"

void project_structs_init(xi_utils* xi){	
	project_structs* project = malloc(sizeof(project_structs));
	xi->project = project;
	// initialize instances of structures here
}

void xisetup(program_state* state, xi_utils* xi){
	project_structs_init(xi);
	// register your systems here
}

void xistart(xi_utils* xi){
	// setup initial state here
}
