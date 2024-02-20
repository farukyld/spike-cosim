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
    // DPI import at src/pkg/cosim_pkg.sv:61:32
    extern void init();
    // DPI import at src/pkg/_1private_dpi_imports_pkg.sv:16:32
    extern void private_get_log_mem_read(const svOpenArrayHandle log_mem_read_o, int* inserted_elements_o, int processor_i);
    // DPI import at src/pkg/_1private_dpi_imports_pkg.sv:22:32
    extern void private_get_log_mem_write(const svOpenArrayHandle log_mem_write_o, int* inserted_elements_o, int processor_i);
    // DPI import at src/pkg/_1private_dpi_imports_pkg.sv:10:32
    extern void private_get_log_reg_write(const svOpenArrayHandle log_reg_write_o, int* inserted_elements_o, int processor_i);
    // DPI import at src/pkg/_1private_dpi_imports_pkg.sv:28:32
    extern void private_get_pc(svBitVecVal* pc_o, int processor_i);
    // DPI import at src/pkg/cosim_pkg.sv:67:31
    extern svBit simulation_completed();
    // DPI import at src/pkg/cosim_pkg.sv:64:32
    extern void step();
    // DPI import at src/pkg/cosim_pkg.sv:71:32
    extern void wait_key();

#ifdef __cplusplus
}
#endif

#endif  // guard
