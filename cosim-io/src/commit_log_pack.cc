#include "commit_log_pack.h"
#include "cosimif.h"
#include <cstring>

size_t pack_commit_log_into_array(uint8_t *buffer, const size_t buffer_size, const state_t &state)
{
  uint8_t *ptr = buffer;
  const uint8_t *end = buffer + buffer_size;

  size_t xreg_count = 0, freg_count = 0, csr_count = 0, mem_count = state.log_mem_write.size();

  // Count reg kinds
  for (const auto &[regid, val] : state.log_reg_write)
  {
    uint8_t kind = regid & 0xF;
    if (kind == 0)
      xreg_count++;
    else if (kind == 1)
      freg_count++;
    else if (kind == 4)
      csr_count++;
  }

  // Header
  if (ptr + 16 > end)
    return 0;
  memcpy(ptr, &state.pc, 8);
  ptr += 8;
  // uint32_t insn_bits = 0;              // You'll likely want to log this from decode
  memcpy(ptr, &state.last_inst_priv, 4);
  ptr += 4;
  *ptr++ = xreg_count;
  *ptr++ = freg_count;
  *ptr++ = csr_count;
  *ptr++ = mem_count;

  // xregs
  for (const auto &[regid, val] : state.log_reg_write)
  {
    if ((regid & 0xF) != 0)
      continue; // xreg
    if (ptr + 9 > end)
      return 0;
    uint8_t regno = regid >> 4;
    memcpy(ptr++, &regno, 1);
    memcpy(ptr, &val.v[0], 8);
    ptr += 8;
  }

  // fregs
  for (const auto &[regid, val] : state.log_reg_write)
  {
    if ((regid & 0xF) != 1)
      continue; // freg
    if (ptr + 17 > end)
      return 0;
    uint8_t regno = regid >> 4;
    memcpy(ptr++, &regno, 1);
    memcpy(ptr, &val.v[0], 16);
    ptr += 16;
  }

  // csrs
  for (const auto &[regid, val] : state.log_reg_write)
  {
    if ((regid & 0xF) != 4)
      continue; // csr
    if (ptr + 10 > end)
      return 0;
    uint16_t csr_addr = regid >> 4;
    memcpy(ptr, &csr_addr, 2);
    ptr += 2;
    memcpy(ptr, &val.v[0], 8);
    ptr += 8;
  }

  // memory
  for (const auto &[vaddr, paddr, value, size] : state.log_mem_write)
  {
    if (ptr + 25 > end)
      return 0;
    memcpy(ptr, &vaddr, 8);
    ptr += 8;
    memcpy(ptr, &paddr, 8);
    ptr += 8;
    *ptr++ = size;
    memcpy(ptr, &value, 8);
    ptr += 8;
  }

  return ptr - buffer;
}

void unpack_commit_log_from_array(const uint8_t *buffer, const size_t buffer_size,
                                  commit_log_reg_t &log_reg_write,
                                  commit_log_mem_t &mem_writes,
                                  reg_t &pc, uint32_t &insn_bits)
{
  const uint8_t *ptr = buffer;
  const uint8_t *end = buffer + buffer_size;

  if (ptr + 16 > end)
    return;

  memcpy(&pc, ptr, 8);
  ptr += 8;
  memcpy(&insn_bits, ptr, 4);
  ptr += 4;
  uint8_t xreg_count = *ptr++;
  uint8_t freg_count = *ptr++;
  uint8_t csr_count = *ptr++;
  uint8_t mem_count = *ptr++;

  for (uint8_t i = 0; i < xreg_count; ++i)
  {
    if (ptr + 9 > end)
      return;
    uint8_t regno = *ptr++;
    freg_t val = {}; // float128_t, has v[2]
    memcpy(&val.v[0], ptr, 8);
    ptr += 8;
    log_reg_write[(regno << 4) | 0] = val;
  }

  for (uint8_t i = 0; i < freg_count; ++i)
  {
    if (ptr + 17 > end)
      return;
    uint8_t regno = *ptr++;
    freg_t val = {};
    memcpy(&val.v[0], ptr, 16);
    ptr += 16;
    log_reg_write[(regno << 4) | 1] = val;
  }

  for (uint8_t i = 0; i < csr_count; ++i)
  {
    if (ptr + 10 > end)
      return;
    uint16_t csr_addr;
    memcpy(&csr_addr, ptr, 2);
    ptr += 2;
    freg_t val = {};
    memcpy(&val.v[0], ptr, 8);
    ptr += 8;
    log_reg_write[(csr_addr << 4) | 4] = val;
  }

  for (uint8_t i = 0; i < mem_count; ++i)
  {
    if (ptr + 25 > end)
      return;
    reg_t vaddr, paddr;
    uint8_t size;
    uint64_t data;
    memcpy(&vaddr, ptr, 8);
    ptr += 8;
    memcpy(&paddr, ptr, 8);
    ptr += 8;
    size = *ptr++;
    memcpy(&data, ptr, 8);
    ptr += 8;
    mem_writes.emplace_back(vaddr, paddr, data, size);
  }
}
