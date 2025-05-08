#ifndef COSIMIF_H
#define COSIMIF_H

#include "sim.h"

extern sim_t* s_ptr;
#ifdef VERILATOR
#include "cosim_ornek_kullanim__Dpi.h"
#else
void init();
void step();
uint8_t simulation_completed();
#endif


#endif
