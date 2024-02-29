package cosim_constants_pkg;
  
  parameter int unsigned XREG_W = 64; // degeri spike'tan ve bizim mimarimizdebn geliyor. integer register'larin genisligi.

  parameter int unsigned FREG_W = 128; // degeri spike'tan geliyor. float register'larin genisligi.

  // register write kayitlarinda hangi register'a yazildigini belirlemede
  // ayni turden (x,f, v, csr) register'larin birbirindene ayirilmasini saglayan identifier.  
  parameter int unsigned REG_KEY_ID_W = 60; // riscv/decode_macros.h'den geliyor
  
  // register write kayitlarinda hangi register'a yazildigini belirlemede
  // register'larin turunu belirleyen kisim (x,f,v,csr)
  parameter int unsigned REG_KEY_TYPE_W = 4; // riscv/decode_macros.h'den geliyor

endpackage