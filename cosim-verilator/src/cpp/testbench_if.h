#ifndef TESTBENCH_IF_H
#define TESTBENCH_IF_H

#ifdef VERILATOR
#include DPI_HEADER
#else
#include <svdpi.h>
extern "C" void get_log_mem_read(svBitVecVal* log_mem_read_o, int* inserted_elements_o);
extern "C" void get_log_mem_write(svBitVecVal* log_mem_write_o, int* inserted_elements_o);
extern "C" void get_log_reg_write(svBitVecVal* log_reg_write_o, int* inserted_elements_o);
extern "C" void get_pc(svBitVecVal* pc_o);
extern "C" void get_priv(unsigned int* priv_o);
extern "C" void get_proc_id(unsigned int* proc_id_o);
extern "C" void wait_key();
#endif
#include "decode.h" // for freg_t and other inttypes

#ifndef UNPACKED_STRUCT_VERILOG

typedef struct {
  uint8_t len;
  uint8_t reserved[7];
  uint64_t wdata; 
  reg_t paddr; 
  reg_t addr; 
} commit_log_mem_item_t;


typedef struct { 
  freg_t value; 
  uint64_t key; 
} commit_log_reg_item_t;


#else
typedef struct {
  reg_t addr;
  reg_t paddr;
  uint64_t wdata;
  uint8_t len;
} commit_log_mem_item_t;


typedef struct {
  reg_t key;
  freg_t value;
} commit_log_reg_item_t;
#endif


#endif // COSIMIF_H
