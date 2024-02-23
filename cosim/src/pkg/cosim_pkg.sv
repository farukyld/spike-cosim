package cosim_pkg;

  parameter int unsigned CommitLogEntries = 16;
  import cosim_constants_pkg::*;

  /* cosim types/enums */

  // float register type
  typedef bit [FREG_W-1:0] freg_t;

  // mimarimizdeki register. pointer'lar icin de kullaniliyor.
  typedef bit [XREG_W-1:0] reg_t;

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
    bit [REG_KEY_ID_W-1:0] xr_fr_vr_id;
    csr_id_e csr_id;
  } reg_id_t;


  typedef struct packed {
    reg_id_t reg_id;
    reg_key_type_e reg_type; // low 4 bit. packed oldugu icin.
  } reg_key_t;


  typedef struct packed {
    reg_key_t key;
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
  import "DPI-C" function void get_log_reg_write(
    output commit_log_reg_item_t log_reg_write_o[CommitLogEntries],
    output int inserted_elements_o,
    input int processor_id = 0
  );

  // son yapilan step'teki memory read islemleri
  import "DPI-C" function  void get_log_mem_read(
    output commit_log_mem_item_t log_mem_read_o[CommitLogEntries],
    output int inserted_elements_o,
    input int processor_id = 0
  );

  // son yapilan step'teki memory write islemleri.
  import "DPI-C" function void get_log_mem_write(
    output commit_log_mem_item_t log_mem_write_o[CommitLogEntries],
    output int inserted_elements_o,
    input int processor_id = 0
  );

  import "DPI-C" function void get_pc(
    output reg_t pc_o,
    input int processor_id = 0
  );
endpackage
