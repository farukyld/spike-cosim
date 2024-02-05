package private_dpi_imports_pkg;
  
  import basic_types_pkg::dpi_word_t;
  import cosim_constants_pkg::LOG_MEM_ITEM_DPI_WORDS;
  import cosim_constants_pkg::LOG_REG_WRITE_ITEM_DPI_WORDS;

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
endpackage