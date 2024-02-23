#ifndef COSIMIF_H
#define COSIMIF_H

#include "../../obj_dir_tb_spike_link/tb_spike_link__Dpi.h"
#include "decode.h"

#ifndef UNPACKED_STRUCT_VERILOG

typedef struct {
  uint8_t len;
  uint8_t reserved[7];
  uint64_t wdata; 
  reg_t addr; 
} commit_log_mem_item_t;


typedef struct { 
  freg_t value; 
  uint64_t key; 
} commit_log_reg_item_t;


#else
typedef struct {
  reg_t addr;
  uint64_t wdata;
  uint8_t len;
} commit_log_mem_item_t;


typedef struct {
  reg_t key;
  freg_t value;
} commit_log_reg_item_t;
#endif


#endif // COSIMIF_H
