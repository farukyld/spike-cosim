#ifndef COSIMIF_H
#define COSIMIF_H

#include "sim.h"

extern sim_t* s_ptr;
void init();
void step();
uint8_t simulation_completed();

#endif
