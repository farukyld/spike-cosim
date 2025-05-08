#include "commit_log_pack.h"
#include "cosimif.h"
#include <cstring>
#include "print_helper.h"
bool debug_commit_log_pack;
// Commit log header layout (aligned, future-aware):
//   pc:           8 bytes
//   intr_taken:   2 bytes
//   body_size:    1 byte
//   priv:         1 byte
//   #freg_writes: 1 byte
//   #xreg_writes: 1 byte
//   #csr_writes:  1 byte
//   #mem_writes:  1 byte
//
// Body layout (f = freg count, x = xreg count, c = csr count, m = mem count):
//   f_values:      16 * f
//   x_values:      8 * x
//   csr_values:    8 * c
//   mem_tuples:    24 * m (vaddr + paddr + value)
//   freg_nos:      1 * f
//   xreg_nos:      1 * x
//   csr_nos:       1 * c
//   mem_lens:      1 * m

constexpr size_t OFFSET_PC = 0;
typedef reg_t pc_val_t;
constexpr size_t SIZE_PC = sizeof(pc_val_t);

constexpr size_t OFFSET_INTERRUPTS = OFFSET_PC + SIZE_PC;
typedef uint16_t interupts_val_t;
constexpr size_t SIZE_INTERRUPTS = sizeof(interupts_val_t);

constexpr size_t OFFSET_BODY_SIZE = OFFSET_INTERRUPTS + SIZE_INTERRUPTS;
typedef uint8_t body_size_t;
constexpr size_t SIZE_BODY_SIZE = sizeof(body_size_t);

constexpr size_t OFFSET_PRIV = OFFSET_BODY_SIZE + SIZE_BODY_SIZE;
typedef uint8_t priv_val_t;
constexpr size_t SIZE_PRIV = sizeof(priv_val_t);

constexpr size_t OFFSET_FREGS_COUNT = OFFSET_PRIV + SIZE_PRIV;
typedef uint8_t fregs_count_t;
constexpr size_t SIZE_FREGS_COUNT = sizeof(fregs_count_t);

constexpr size_t OFFSET_XREGS_COUNT = OFFSET_FREGS_COUNT + SIZE_FREGS_COUNT;
typedef uint8_t xregs_count_t;
constexpr size_t SIZE_XREGS_COUNT = sizeof(xregs_count_t);

constexpr size_t OFFSET_CSRS_COUNT = OFFSET_XREGS_COUNT + SIZE_XREGS_COUNT;
typedef uint8_t csrs_count_t;
constexpr size_t SIZE_CSRS_COUNT = sizeof(csrs_count_t);

constexpr size_t OFFSET_MEM_WRITES_COUNT = OFFSET_CSRS_COUNT + SIZE_CSRS_COUNT;
typedef uint8_t mem_writes_count_t;
constexpr size_t SIZE_MEM_WRITES_COUNT = sizeof(mem_writes_count_t);

constexpr size_t HEADER_SIZE = OFFSET_MEM_WRITES_COUNT + SIZE_MEM_WRITES_COUNT;
const std::vector<size_t> HEADER_FORMAT = {8, 2, 1, 1, 1, 1, 1, 1};

typedef freg_t freg_value_t;
constexpr size_t SIZE_FREG_VALUE = sizeof(freg_value_t);

typedef reg_t xreg_value_t;
constexpr size_t SIZE_XREG_VALUE = sizeof(xreg_value_t);

typedef reg_t csr_value_t;
constexpr size_t SIZE_CSR_VALUE = sizeof(csr_value_t);

typedef struct mem_triple_t
{
  reg_t vaddr;
  reg_t paddr;
  reg_t value;
} mem_triple_t;
constexpr size_t SIZE_MEM_TRIPLE = sizeof(mem_triple_t);

typedef uint8_t freg_no_t;
constexpr size_t SIZE_FREG_NO = sizeof(freg_no_t);

typedef uint8_t xreg_no_t;
constexpr size_t SIZE_XREG_NO = sizeof(xreg_no_t);

typedef uint16_t csr_no_t;
constexpr size_t SIZE_CSR_NO = sizeof(csr_no_t);

typedef uint8_t mem_len_t;
constexpr size_t SIZE_MEM_LEN = sizeof(mem_len_t);

constexpr size_t ASSUMED_MAX_F_WRITES = 1;
constexpr size_t ASSUMED_MAX_X_WRITES = 2;
constexpr size_t ASSUMED_MAX_CSR_WRITES = 10;
constexpr size_t ASSUMED_MAX_MEM_WRITES = 2;

constexpr size_t ASSUMED_MAX_BODY_SIZE =
    ASSUMED_MAX_F_WRITES * (SIZE_FREG_VALUE + SIZE_FREG_NO) +
    ASSUMED_MAX_X_WRITES * (SIZE_XREG_VALUE + SIZE_XREG_NO) +
    ASSUMED_MAX_CSR_WRITES * (SIZE_CSR_VALUE + SIZE_CSR_NO) +
    ASSUMED_MAX_MEM_WRITES * (SIZE_MEM_TRIPLE + SIZE_MEM_LEN);

constexpr size_t ASSUMED_MAX_COMMIT_LOG_SIZE = HEADER_SIZE + ASSUMED_MAX_BODY_SIZE;

size_t pack_commit_log_into_array(uint8_t *buffer, const size_t buffer_size, const state_t &state)
{
  uint8_t *ptr = buffer;

  size_t xreg_count = 0, freg_count = 0, csr_count = 0, mem_count = 0;

  // Value arrays (ordered in body)
  freg_value_t freg_values[ASSUMED_MAX_F_WRITES];
  xreg_value_t xreg_values[ASSUMED_MAX_X_WRITES];
  csr_value_t csr_values[ASSUMED_MAX_CSR_WRITES];
  mem_triple_t mem_triples[ASSUMED_MAX_MEM_WRITES];

  // Register no arrays
  freg_no_t freg_nos[ASSUMED_MAX_F_WRITES];
  xreg_no_t xreg_nos[ASSUMED_MAX_X_WRITES];
  csr_no_t csr_nos[ASSUMED_MAX_CSR_WRITES];

  // Memory metadata
  mem_len_t mem_lens[ASSUMED_MAX_MEM_WRITES];

  // cozumleme asamasi
  // if (debug_commit_log_pack)
  //   printf(YELLOW "cozumleme\n" DEF_COLOR);
  // Count reg kinds, and accumulate values
  for (const auto &[regid, val] : state.log_reg_write)
  {
    uint8_t kind = regid & 0xF;
    if (likely(kind == 0))
    {
      xreg_values[xreg_count] = val.v[0];
      xreg_nos[xreg_count] = regid >> 4;
      xreg_count++;
    }
    else if (kind == 1)
    {
      freg_values[freg_count] = val;
      freg_nos[freg_count] = regid >> 4;
      freg_count++;
    }
    else if (kind == 4)
    {
      csr_values[csr_count] = val.v[0];
      csr_nos[csr_count] = regid >> 4;
      // if (debug_commit_log_pack)
      //   printf("csr%X: %lX\n", csr_nos[csr_count], csr_values[csr_count]);
      csr_count++;
    }
  }

  for (const auto &[vaddr, paddr, value, len] : state.log_mem_write)
  {
    mem_triples[mem_count] = {.vaddr = vaddr, .paddr = paddr, .value = value};
    mem_lens[mem_count] = len;
    mem_count++;
  }

  // birlestirme asamasi

  // Header
  if (unlikely(HEADER_SIZE > buffer_size))
    return 0;

  *(pc_val_t *)ptr = state.pc;
  ptr += SIZE_PC;

  *(interupts_val_t *)ptr = 0;
  ptr += SIZE_INTERRUPTS;

  // calculate body size
  auto freg_part_size = freg_count * (SIZE_FREG_VALUE + SIZE_FREG_NO);
  auto xreg_part_size = xreg_count * (SIZE_XREG_VALUE + SIZE_XREG_NO);
  auto csr_part_size = csr_count * (SIZE_CSR_VALUE + SIZE_CSR_NO);
  auto mem_part_size = mem_count * (SIZE_MEM_TRIPLE + SIZE_MEM_LEN);

  size_t body_size = freg_part_size + xreg_part_size + csr_part_size + mem_part_size;
  if (unlikely(body_size > ASSUMED_MAX_BODY_SIZE || body_size + HEADER_SIZE > buffer_size))
  {
    return 0;
  }
  *(body_size_t *)ptr = (body_size_t)body_size;
  ptr += SIZE_BODY_SIZE;

  *(priv_val_t *)ptr = (priv_val_t)state.last_inst_priv;
  ptr += SIZE_PRIV;

  *(fregs_count_t *)ptr = (fregs_count_t)freg_count;
  ptr += SIZE_FREGS_COUNT;
  *(xregs_count_t *)ptr = (xregs_count_t)xreg_count;
  ptr += SIZE_XREGS_COUNT;
  *(csrs_count_t *)ptr = (csrs_count_t)csr_count;
  ptr += SIZE_CSRS_COUNT;
  *(mem_writes_count_t *)ptr = (mem_writes_count_t)mem_count;
  ptr += SIZE_MEM_WRITES_COUNT;

  for (size_t i = 0; i < freg_count; i++)
  {
    ((freg_value_t *)ptr)[i] = freg_values[i];
    ptr += SIZE_FREG_VALUE;
  }

  for (size_t i = 0; i < xreg_count; i++)
  {
    ((xreg_value_t *)ptr)[i] = xreg_values[i];
    ptr += SIZE_XREG_VALUE;
  }

  for (size_t i = 0; i < csr_count; i++)
  {
    ((csr_value_t *)ptr)[i] = csr_values[i];
    ptr += SIZE_CSR_VALUE;
  }

  for (size_t i = 0; i < mem_count; i++)
  {
    ((mem_triple_t *)ptr)[i] = mem_triples[i];
    ptr += SIZE_MEM_TRIPLE;
  }

  // regids, mem_write lengths
  for (size_t i = 0; i < freg_count; i++)
  {
    ((freg_no_t *)ptr)[i] = freg_nos[i];
    ptr += SIZE_FREG_NO;
  }

  for (size_t i = 0; i < xreg_count; i++)
  {
    ((xreg_no_t *)ptr)[i] = xreg_nos[i];
    ptr += SIZE_XREG_NO;
  }

  for (size_t i = 0; i < csr_count; i++)
  {
    ((csr_no_t *)ptr)[i] = csr_nos[i];
    ptr += SIZE_CSR_NO;
  }

  for (size_t i = 0; i < mem_count; i++)
  {
    ((mem_len_t *)ptr)[i] = mem_lens[i];
    ptr += SIZE_MEM_LEN;
  }

  return body_size + HEADER_SIZE;
}

bool unpack_commit_log_header(const uint8_t *buffer, const size_t buffer_size,
                              reg_t &pc,
                              uint16_t &interrupts_taken,
                              uint8_t &body_size,
                              uint8_t &priv,
                              uint8_t &freg_count,
                              uint8_t &xreg_count,
                              uint8_t &csr_count,
                              uint8_t &mem_count,
                              const uint8_t *&body_ptr)
{
  // TODO: buffer size'in burada sadece kontrol edilmek icin bulunmasi normal mi?
  // cagiran kisi header size'i bilmesi gerekiyor mu?
  // eger bilmesine izin veriyorsak ona gore davranmak zorunda oldugunu mu beklemeliyiz?
  // sabit uzunluklu olmasigereken header'in adinin buffer olmasi normal mi?
  if (unlikely(buffer_size < HEADER_SIZE))
    return false;

  const uint8_t *ptr = buffer;

  pc = *(const pc_val_t *)ptr;
  ptr += SIZE_PC;

  interrupts_taken = *(const interupts_val_t *)ptr;
  ptr += SIZE_INTERRUPTS;

  body_size = *(const body_size_t *)ptr;
  ptr += SIZE_BODY_SIZE; // skip body_size, not used directly here

  priv = *(const priv_val_t *)ptr;
  ptr += SIZE_PRIV;

  freg_count = *(const fregs_count_t *)ptr;
  ptr += SIZE_FREGS_COUNT;

  xreg_count = *(const xregs_count_t *)ptr;
  ptr += SIZE_XREGS_COUNT;

  csr_count = *(const csrs_count_t *)ptr;
  ptr += SIZE_CSRS_COUNT;

  mem_count = *(const mem_writes_count_t *)ptr;
  ptr += SIZE_MEM_WRITES_COUNT;

  body_ptr = ptr;
  return true;
}

bool unpack_commit_log_body(const uint8_t *body_ptr,
                            const size_t buffer_size, // to validate if needed
                            uint8_t freg_count,
                            uint8_t xreg_count,
                            uint8_t csr_count,
                            uint8_t mem_count,
                            commit_log_reg_t &log_reg_write,
                            commit_log_mem_t &log_mem_write)
{
  const uint8_t *ptr = body_ptr;

  const freg_value_t *freg_values = (const freg_value_t *)ptr;
  ptr += freg_count * SIZE_FREG_VALUE;

  const xreg_value_t *xreg_values = (const xreg_value_t *)ptr;
  ptr += xreg_count * SIZE_XREG_VALUE;

  const csr_value_t *csr_values = (const csr_value_t *)ptr;
  ptr += csr_count * SIZE_CSR_VALUE;

  const mem_triple_t *mem_triples = (const mem_triple_t *)ptr;
  ptr += mem_count * SIZE_MEM_TRIPLE;

  const freg_no_t *freg_nos = (const freg_no_t *)ptr;
  ptr += freg_count * SIZE_FREG_NO;

  const xreg_no_t *xreg_nos = (const xreg_no_t *)ptr;
  ptr += xreg_count * SIZE_XREG_NO;

  const csr_no_t *csr_nos = (const csr_no_t *)ptr;
  ptr += csr_count * SIZE_CSR_NO;

  const mem_len_t *mem_lens = (const mem_len_t *)ptr;
  ptr += mem_count * SIZE_MEM_LEN;

  // Optional: check for overflow
  if ((size_t)(ptr - body_ptr) > buffer_size)
    return false;

  log_reg_write.clear();
  for (size_t i = 0; i < xreg_count; i++)
  {
    reg_t id = (xreg_nos[i] << 4) | 0;
    log_reg_write[id] = {{xreg_values[i], 0}};
  }

  for (size_t i = 0; i < freg_count; ++i)
  {
    reg_t id = (freg_nos[i] << 4) | 1;
    log_reg_write[id] = freg_values[i];
  }

  for (size_t i = 0; i < csr_count; ++i)
  {
    reg_t id = (csr_nos[i] << 4) | 4;
    log_reg_write[id] = {{csr_values[i], 0}};
  }

  log_mem_write.clear();
  for (size_t i = 0; i < mem_count; ++i)
  {
    const auto &[vaddr, paddr, value] = mem_triples[i];
    log_mem_write.emplace_back(vaddr, paddr, value, mem_lens[i]);
  }

  return true;
}
