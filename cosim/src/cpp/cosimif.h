#ifndef COSIMIF_H
#define COSIMIF_H

#include "../../obj_dir_tb_spike_link/tb_spike_link__Dpi.h"
#include "decode.h"


#define DPI_W (sizeof(svBitVecVal)*8)
#define REGT_W (sizeof(reg_t)*8)
#define UINT64T_W (sizeof(uint64_t)*8)
#define UINT8T_W (sizeof(uint8_t)*8)

// direkt sizeof(struct)/sizeof(svBitVecVal) yapinca 24 byte veriyor.
#define CMT_LOG_MEM_ITEM_DPI_WORDS (REGT_W/DPI_W + UINT64T_W/DPI_W + UINT8T_W/DPI_W + 1)


typedef struct {
  reg_t addr;
  uint64_t wdata;
  uint8_t len;
} commit_log_mem_item_t;


#define FREGT_W (sizeof(freg_t)*8)
// oncekinde oyle yaptigim icin bunda da ayni sekilde yapiyorum.
#define CMT_LOG_REG_ITEM_DPI_WORDS (REGT_W/DPI_W + FREGT_W/DPI_W)


typedef struct {
  reg_t key;
  freg_t value;
} commit_log_reg_item_t;

#endif // COSIMIF_H
