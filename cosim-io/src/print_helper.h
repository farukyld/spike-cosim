#ifndef PRINT_HELPER_H
#define PRINT_HELPER_H

#include <inttypes.h>
#include <stddef.h>
#include <vector>

void print_sliced_hex(const uint8_t *data, size_t data_len,
                      const std::vector<size_t> &slice_lengths = {},
                      char delim = ':');

#endif // PRINT_HELPER_H
