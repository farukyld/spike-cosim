#ifndef PRINT_HELPER_H
#define PRINT_HELPER_H

#include <inttypes.h>
#include <stddef.h>
#include <vector>
#include "processor.h"

#define DEF_COLOR "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GRAY "\033[90m"
#define BRIGHT_RED "\033[91m" 
#define BRIGHT_GREEN "\033[92m" 
#define BRIGHT_YELLOW "\033[93m" 
#define BRIGHT_BLUE "\033[94m" 
#define BRIGHT_MAGENTA "\033[95m" 
#define BRIGHT_CYAN "\033[96m" 
#define BRIGHT_WHITE "\033[97m" 

void print_log(commit_log_reg_t log_reg_write, commit_log_mem_t log_mem_write);

void print_sliced_hex(const uint8_t *data, size_t data_len,
                      const std::vector<size_t> &slice_lengths = {},
                      char delim = ':');

#endif // PRINT_HELPER_H
