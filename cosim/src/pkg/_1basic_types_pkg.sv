package basic_types_pkg;
  import cosim_constants_pkg::DPI_W;
  import cosim_constants_pkg::FREG_W;
  import cosim_constants_pkg::XREG_W;
  
  // vverilator dpi gerceklemesinde svPutArrElem.. fonksiyonlari ile tek seferde
  // gecilebilen data
  typedef bit [DPI_W-1:0]  dpi_word_t;

  // float register type
  typedef bit [FREG_W-1:0] freg_t;

  // mimarimizdeki word
  typedef bit [XREG_W-1:0] reg_t;
endpackage