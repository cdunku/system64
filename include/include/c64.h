#pragma once

#include "c64_bus.h"
#include "display.h"

extern SDL_Color c64_colors[16];


// Initialise SDL display and whole system
c64_t *c64_init(void);

void c64_run(c64_t* c64);

