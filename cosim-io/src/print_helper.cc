#include "print_helper.h"

#include <cstdint>
#include <vector>
#include <cstddef>
#include <iostream>

// Converts 4-bit nibble to hex character
char nibble_to_hex(uint8_t nibble)
{
    return nibble < 10 ? ('0' + nibble) : ('A' + nibble - 10);
}

// Prints a byte as two hex digits (MSB first)
void print_byte_hex(uint8_t byte)
{
    putc(nibble_to_hex(byte >> 4), stderr);
    putc(nibble_to_hex(byte & 0x0F), stderr);
}

// Main function: prints buffer with optional slicing and delimiter
void print_sliced_hex(const uint8_t *data, size_t data_len,
                      const std::vector<size_t> &slice_lengths,
                      char delim)
{
    size_t index = 0;
    size_t slice_idx = 0;

    while (index < data_len)
    {
        size_t slice_len;

        // Use given slice if available, otherwise default to 8
        if (slice_idx < slice_lengths.size())
        {
            slice_len = slice_lengths[slice_idx];
        }
        else
        {
            slice_len = 8;
        }

        // Adjust slice size if it overflows buffer
        if (index + slice_len > data_len)
        {
            slice_len = data_len - index;
        }

        // Print slice in reverse (MSB left)
        for (size_t i = 0; i < slice_len; ++i)
        {
            print_byte_hex(data[index + slice_len - 1 - i]);
        }

        index += slice_len;
        ++slice_idx;

        // Print delimiter if not the last slice
        if (index < data_len)
        {
            putc(delim, stderr);
        }
    }

    putc('\n', stderr);
}

void print_log(commit_log_reg_t log_reg_write, commit_log_mem_t log_mem_write)
{
    if (log_reg_write.size() != 0)
        fprintf(stderr, YELLOW "reg writes:\n" DEF_COLOR);
    for (const auto &[reg_no, value] : log_reg_write)
    {
        auto reg_type = reg_no & 0xf;
        if (reg_type == 0)
            fputc('x', stderr);
        else if (reg_type == 1)
            fputc('f', stderr);
        else
            fputs("csr", stderr);

        auto actual_regno = reg_no >> 4;
        
        fprintf(stderr, "%lX: %016lX_%016lX\n", actual_regno, value.v[1], value.v[0]);
    }
    if (log_mem_write.size() != 0)
        fprintf(stderr, YELLOW "mem writes:\n" DEF_COLOR);
    for (const auto &[vaddr, paddr, value, len] : log_mem_write)
    {
        fprintf(stderr, "%lX/%lX: %lX (%u)\n", vaddr, paddr, value, len);
    }
}
