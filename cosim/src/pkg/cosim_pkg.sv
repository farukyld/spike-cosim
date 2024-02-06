package cosim_pkg;

  parameter int unsigned CommitLogEntries = 16;
  import cosim_constants_pkg::*;

  /* cosim types/enums */
  typedef basic_types_pkg::reg_t reg_t; // bunlari disariya gostermek istiyor muyum?
  typedef basic_types_pkg::freg_t freg_t; // eger istemiyorsam sadece import yapmaliyim.
  // typedef yapmamaliyim

  import basic_types_pkg::dpi_word_t; // bunu disariya gostermek istemiyorum.


  // register key type enum
  // from riscv/decode_macros.h
  typedef enum bit [REG_KEY_TYPE_W-1:0] {
    XREG      = REG_KEY_TYPE_W'('b0000),
    FREG      = REG_KEY_TYPE_W'('b0001),
    VREG      = REG_KEY_TYPE_W'('b0010),
    VREG_HINT = REG_KEY_TYPE_W'('b0011),
    CSR       = REG_KEY_TYPE_W'('b0100)
  } reg_key_type_e;

  // csr id enum cok buyuk olduğu icin ayri bir dosyaya koydum, aslinda burda durmasi gerekiyordu

  // csr'larin ozel id'leri var
  typedef csr_ids_pkg::csr_id_e csr_id_e; // bunu disariya gostermek istiyorum.

  // diger id'ler duz 0'dan 31'e.
  typedef union packed {
    bit [REG_KEY_ID_W-1:0] int_float_vec;
    csr_id_e csr;
  } reg_id_t;


  typedef struct packed {
    reg_id_t reg_id;
    reg_key_type_e reg_type; // low 4 bit.
    // packed olunca low 4 bit. unpacked olursa verilog std. herhangi bir yerde olabilir der.
    // unpacked, hafizada duzenli yayilmak zorunda degil.
    // vverilator'de packed olunca low 4 bit. (olmasi gereken)
    // unpacked olunca high 4 bit. (std tarafindan zorunlu tutulmuyor)
  } key_parts_t;


  typedef union packed {
    // 64 bit key {60 bit id, 4 bit type}
    reg_t key;
    key_parts_t key_parts;
  } reg_key_t;


  typedef struct packed {
    reg_key_t key; // packed'in icinde unpacked koyamiyoruz, o reg_key_t union'u da packed.
    freg_t value;
  } commit_log_reg_item_t;

  
  typedef  struct packed {
    reg_t addr;
    reg_t wdata;
    byte  len;
  } commit_log_mem_item_t;

  // spike simulation olusturur. command line argumanlarin hangi dosyadan okundugunu
  // cosim/src/cpp/cosimif.cc:init() fonksiyonunun tanimindan degistirebilirsiniz.
  import "DPI-C" function void init();

  // spike simulation'u bir adim ilerletir.
  import "DPI-C" function void step();

  // simulation'da kosan kod, exitcode gonderdiyse 1 dondurur.
  import "DPI-C" function bit simulation_completed();

  // cosim'in bir parcasi degil, adim adim ilerletip incelemek icin koydum. 
  // "testbench'imde kullaniyorum.devam etmek icin bir tusa basiniz" yapmaya yariyor.
  import "DPI-C" function void wait_key();

  // son simulation adiminda yapilan register write kayitlarini output parametresine yazar.
  // kac tane eleman eklendiyse sayisini inserted_elements_o'ya yazar.
  function void get_log_reg_write(
    output commit_log_reg_item_t log_reg_write_o[CommitLogEntries],
    output int inserted_elements_o
  );
    // burda private_get_log_reg_write_from_c cagirilacak
    // ciktisi log_reg_write_o'ya yazilacak
    dpi_word_t log_reg_write_from_c [CommitLogEntries][LOG_REG_WRITE_ITEM_DPI_WORDS];
    int n;

    private_dpi_imports_pkg::private_get_log_reg_write(log_reg_write_from_c, inserted_elements_o, 0);

    for (int ii = 0; ii < inserted_elements_o; ii = ii + 1) begin: log_reg_write_itr
      log_reg_write_o[ii].key.key = pack_2x32_to64le(log_reg_write_from_c[ii][0:1]);
      log_reg_write_o[ii].value = pack_4x32_to128le(log_reg_write_from_c[ii][2:5]);
    end
  endfunction

  // son yapilan step'teki memory read islemleri
  function void get_log_mem_read(
    output commit_log_mem_item_t log_mem_read_o[CommitLogEntries],
    output int inserted_elements_o
  );
    // burda private_get_log_mem_read_from_c cagirilacak
    // ciktisi log_mem_read_o'ya yazilacak
    dpi_word_t log_mem_read_from_c [CommitLogEntries][LOG_MEM_ITEM_DPI_WORDS];
    int n;

    private_dpi_imports_pkg::private_get_log_mem_read(log_mem_read_from_c, inserted_elements_o, 0);

    for (int ii = 0; ii < inserted_elements_o; ii = ii + 1) begin: log_mem_read_itr
      log_mem_read_o[ii].addr = pack_2x32_to64le(log_mem_read_from_c[ii][0:1]);
      log_mem_read_o[ii].wdata = pack_2x32_to64le(log_mem_read_from_c[ii][2:3]);
      // !!! truncaiton hatasi verebilir
      log_mem_read_o[ii].len = log_mem_read_from_c[ii][4][7:0];
    end

  endfunction

  // son yapilan step'teki memory write islemleri.
  function void get_log_mem_write(
    output commit_log_mem_item_t log_mem_write_o[CommitLogEntries],
    output int inserted_elements_o
  );
    // burda private_get_log_mem_write_from_c cagirilacak
    // ciktisi log_mem_write_o'ya yazilacak
    dpi_word_t log_mem_write_from_c [CommitLogEntries][LOG_MEM_ITEM_DPI_WORDS];
    int n;

    private_dpi_imports_pkg::private_get_log_mem_write(log_mem_write_from_c, inserted_elements_o, 0);

    for (int ii = 0; ii < inserted_elements_o; ii = ii + 1) begin: log_mem_write_itr
      log_mem_write_o[ii].addr = pack_2x32_to64le(log_mem_write_from_c[ii][0:1]);
      log_mem_write_o[ii].wdata = pack_2x32_to64le(log_mem_write_from_c[ii][2:3]);
      // !!! truncaiton hatasi verebilir
      log_mem_write_o[ii].len = log_mem_write_from_c[ii][4][7:0];
    end

  endfunction

  
  // automatic -> defined variables are auto
  // default (static) -> defined variables are shared among
  // concurrent calls of that function. 13.4.2


  function automatic logic [63:0] pack_2x32_to64le (
    logic [31:0] parts[0:1]
  );
    pack_2x32_to64le = {parts[1], parts[0]};
  endfunction

  function automatic logic [127:0] pack_4x32_to128le (
    logic [31:0] parts[0:3]
  );
    pack_4x32_to128le = {parts[3], parts[2], parts[1], parts[0]};
  endfunction


  task automatic compare_log_reg_write(
    input commit_log_reg_item_t correct_log_reg_write [CommitLogEntries],
    input int entry_count,
    input freg_t verilog_map [reg_t]
  );
    int ii;
    for (ii = 0; ii < entry_count; ii = ii + 1) begin: comparison_itr

    end
  endtask

endpackage
