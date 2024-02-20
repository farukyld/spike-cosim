package private_dpi_imports_pkg;
  
  import cosim_constants_pkg::LOG_MEM_ITEM_DPI_WORDS;
  import cosim_constants_pkg::LOG_REG_WRITE_ITEM_DPI_WORDS;
  import cosim_constants_pkg::XREG_DPI_WORDS;

  import cosim_constants_pkg::DPI_W;
  typedef bit [DPI_W-1:0]  dpi_word_t;

  import "DPI-C" function void private_get_log_reg_write(
    output dpi_word_t log_reg_write_o [][LOG_REG_WRITE_ITEM_DPI_WORDS],
    output int inserted_elements_o,
    input int processor_i
  );

  import "DPI-C" function void private_get_log_mem_read(
    output dpi_word_t log_mem_read_o[][LOG_MEM_ITEM_DPI_WORDS],
    output int inserted_elements_o,
    input int processor_i
  );

  import "DPI-C" function void private_get_log_mem_write(
    output dpi_word_t log_mem_write_o [][LOG_MEM_ITEM_DPI_WORDS],
    output int inserted_elements_o,
    input int processor_i
  );

  import "DPI-C" function void private_get_pc(
    output dpi_word_t [XREG_DPI_WORDS-1:0] pc_o,
    input int processor_i
  );

endpackage