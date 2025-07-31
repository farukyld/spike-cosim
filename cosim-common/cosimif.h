#ifndef COSIMIF_H
#define COSIMIF_H

#include "sim.h"

extern sim_t* s_ptr;
#ifdef VERILATOR
#include DPI_HEADER
#else
extern "C" void init();
extern "C" void step();
extern "C" uint8_t simulation_completed();
#endif


#endif
