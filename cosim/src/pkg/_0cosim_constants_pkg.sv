package cosim_constants_pkg;
  
  parameter int unsigned XREG_W = 64;
  parameter int unsigned FREG_W = 128; // degeri spike'tan geliyor
  parameter int unsigned DPI_W = 32; // degeri svdpi.h'den geliyor
  parameter int unsigned LOG_REG_WRITE_ITEM_DPI_WORDS = XREG_W/DPI_W + FREG_W/DPI_W;
  parameter int unsigned LOG_MEM_ITEM_DPI_WORDS = 2*XREG_W/DPI_W + 1;
  parameter int unsigned REG_KEY_ID_W = 60; // riscv/decode_macros.h'den geliyor
  parameter int unsigned REG_KEY_TYPE_W = 4; // riscv/decode_macros.h'den geliyor

endpackage