#include "bitreader.h"

struct bit_reader new_bit_reader(unsigned char *source, unsigned long source_length)
{
    struct bit_reader bit_reader;
    bit_reader.source = source;
    bit_reader.source_length = source_length << 3;
    bit_reader.position = 0;
    bit_reader.buffer_position = 0;
    bit_reader.lock_position = false;

    if (source_length == 1)
        bit_reader.buffer = source[0] << 8;
    else if (source_length >= 2)
        bit_reader.buffer = (source[0] << 8) | source[1];

    return bit_reader;
}

unsigned char read_bits(struct bit_reader *bit_reader, int bits)
{
    if (bits == 0) return 0;
    if (bit_reader->position - bit_reader->buffer_position >= 8)
    {
        // [delros] gotta avoid divisions, they're expensive
        unsigned long byte_position = bit_reader->position >> 3;

        bit_reader->buffer <<= 8;
        bit_reader->buffer |= bit_reader->source[byte_position + 1];

        bit_reader->buffer_position = bit_reader->position;
    }

    unsigned char bit_position = bit_reader->position - bit_reader->buffer_position;

    unsigned short mask = 0xFFFF >> bit_position;
    mask ^= mask >> bits;

    unsigned char result = (bit_reader->buffer & mask) >> (16 - bit_position - bits);

    bit_reader->position += bits;

    return result;
}
