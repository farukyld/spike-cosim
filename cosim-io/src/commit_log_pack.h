#ifndef COMMIT_LOG_PACK_H
#define COMMIT_LOG_PACK_H

#include "cosimif.h"
#include <cstddef>

size_t pack_commit_log_into_array(uint8_t *buffer, const size_t buffer_size, const state_t &state);

void unpack_commit_log_from_array(const uint8_t *buffer, const size_t buffer_size, commit_log_reg_t &log_reg_write, commit_log_mem_t &mem_writes, reg_t &pc, uint32_t &insn_bits);

#endif