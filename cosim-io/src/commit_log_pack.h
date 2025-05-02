#ifndef COMMIT_LOG_PACK_H
#define COMMIT_LOG_PACK_H

#include "cosimif.h"
#include <cstddef>

const extern size_t HEADER_SIZE;
const extern size_t ASSUMED_MAX_BODY_SIZE;
const extern size_t ASSUMED_MAX_COMMIT_LOG_SIZE;

size_t pack_commit_log_into_array(uint8_t *buffer, const size_t buffer_size, const state_t &state);

bool unpack_commit_log_header(const uint8_t *buffer, const size_t buffer_size,
                              reg_t &pc,
                              uint16_t &interrupts_taken,
                              uint8_t &body_size,
                              uint8_t &priv,
                              uint8_t &freg_count,
                              uint8_t &xreg_count,
                              uint8_t &csr_count,
                              uint8_t &mem_count,
                              const uint8_t *&body_ptr);

bool unpack_commit_log_body(const uint8_t *body_ptr,
                            const size_t buffer_size, // to validate if needed
                            uint8_t freg_count,
                            uint8_t xreg_count,
                            uint8_t csr_count,
                            uint8_t mem_count,
                            commit_log_reg_t &log_reg_write,
                            commit_log_mem_t &log_mem_write);

#endif