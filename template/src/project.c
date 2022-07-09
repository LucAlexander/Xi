#include <stdio.h>

#include "../../../xi.h"
#include "../../../xi_config.h"

#include "project.h"
#include "project-components.h"

void project_structs_init(xi_utils* xi){	
	project_structs* project = malloc(sizeof(project_structs));
	xi->project = project;
	// initialize instances of game structures here
}

void xisetup(xi_utils* xi){
	// register your systems here
}

void xistart(xi_utils* xi){
	// setup initial game state here
}
