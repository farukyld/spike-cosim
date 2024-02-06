package cosim_constants_pkg;
  
  parameter int unsigned XREG_W = 64; // degeri spike'tan ve bizim mimarimizdebn geliyor. integer register'larin genisligi.

  parameter int unsigned FREG_W = 128; // degeri spike'tan geliyor. float register'larin genisligi.

  // degeri svdpi.h'den geliyor.
  // DPI'in verilator gerceklemesinde svPutArrElem.. fonksiyonlarini kulalnarak tek seferde aktarilabilen bit sayisi.
  parameter int unsigned DPI_W = 32; 
  
  // register write kayitlarinin dpi ile cpp'den sv'ye gecilmesi icin kac dpi word kullanilmasi gerektigi.
  parameter int unsigned LOG_REG_WRITE_ITEM_DPI_WORDS = XREG_W/DPI_W + FREG_W/DPI_W;
  
  // mem read/write kayitlarinin dpi ile cpp'den sv'ye gecilmesi icin kac dpi word kullanilmasi gerektigi.
  parameter int unsigned LOG_MEM_ITEM_DPI_WORDS = 2*XREG_W/DPI_W + 1;
  
  // register write kayitlarinda hangi register'a yazildigini belirlemede
  // ayni turden (x,f, v, csr) register'larin birbirindene ayirilmasini saglayan identifier.  
  parameter int unsigned REG_KEY_ID_W = 60; // riscv/decode_macros.h'den geliyor
  
  // register write kayitlarinda hangi register'a yazildigini belirlemede
  // register'larin turunu belirleyen kisim (x,f,v,csr)
  parameter int unsigned REG_KEY_TYPE_W = 4; // riscv/decode_macros.h'den geliyor

endpackage