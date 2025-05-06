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
    std::putchar(nibble_to_hex(byte >> 4));
    std::putchar(nibble_to_hex(byte & 0x0F));
}

// Main function: prints buffer with optional slicing and delimiter
void print_sliced_hex(const uint8_t *data, size_t data_len,
                      const std::vector<size_t> &slice_lengths = {},
                      char delim = ':')
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
            std::putchar(delim);
        }
    }

    std::putchar('\n');
}
