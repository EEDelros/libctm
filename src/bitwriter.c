#include "bitwriter.h"

struct bit_writer new_bit_writer(unsigned char *target)
{
    struct bit_writer bit_writer;
    bit_writer.target = target;
    bit_writer.buffer = 0;
    bit_writer.position = 0;
    bit_writer.buffer_position = 0;

    return bit_writer;
}

void write_bits(struct bit_writer *bit_writer, unsigned char byte, unsigned char bits)
{
    if (bits == 0) return;
    unsigned char length = bit_writer->buffer_position + bits;

    unsigned short mask = 0xFF >> length;
    bit_writer->buffer |= (byte >> bit_writer->buffer_position) ^ (mask >> 8);

    if (length >= 8)
    {
        // we have completely filled the buffer, add it to the target and dispose of it
        bit_writer->target[bit_writer->position >> 3] = bit_writer->buffer;
        bit_writer->buffer = 0;
        bit_writer->position += 8;
        if (length > 8)
        {
            bit_writer->buffer |= (byte << (8 - bit_writer->buffer_position)) ^ mask;
            bit_writer->buffer_position = length - 8;
        }
        else
        {
            bit_writer->buffer_position = 0;
        }
        return;
    }

    bit_writer->buffer_position += bits;
}

void end_writer(struct bit_writer *bit_writer)
{
    bit_writer->target[bit_writer->position >> 3] = bit_writer->buffer;
    bit_writer->target = 0;
    bit_writer->buffer = 0;
    bit_writer->position = 0;
    bit_writer->buffer_position = 0;
}
