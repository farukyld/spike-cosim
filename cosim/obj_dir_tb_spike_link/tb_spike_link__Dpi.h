// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Prototypes for DPI import and export functions.
//
// Verilator includes this file in all generated .cpp files that use DPI functions.
// Manually include this file where DPI .c import functions are declared to ensure
// the C functions match the expectations of the DPI imports.

#ifndef VERILATED_TB_SPIKE_LINK__DPI_H_
#define VERILATED_TB_SPIKE_LINK__DPI_H_  // guard

#include "svdpi.h"

#ifdef __cplusplus
extern "C" {
#endif


    // DPI IMPORTS
    // DPI import at src/pkg/cosim_pkg.sv:77:33
    extern void get_log_mem_read(svBitVecVal* log_mem_read_o, int* inserted_elements_o, int processor_id);
    // DPI import at src/pkg/cosim_pkg.sv:84:32
    extern void get_log_mem_write(svBitVecVal* log_mem_write_o, int* inserted_elements_o, int processor_id);
    // DPI import at src/pkg/cosim_pkg.sv:70:32
    extern void get_log_reg_write(svBitVecVal* log_reg_write_o, int* inserted_elements_o, int processor_id);
    // DPI import at src/pkg/cosim_pkg.sv:90:32
    extern void get_pc(svBitVecVal* pc_o, int processor_id);
    // DPI import at src/pkg/cosim_pkg.sv:56:32
    extern void init();
    // DPI import at src/pkg/cosim_pkg.sv:62:31
    extern svBit simulation_completed();
    // DPI import at src/pkg/cosim_pkg.sv:59:32
    extern void step();
    // DPI import at src/pkg/cosim_pkg.sv:66:32
    extern void wait_key();

#ifdef __cplusplus
}
#endif

#endif  // guard
