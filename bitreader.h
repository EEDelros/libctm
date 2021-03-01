#include <stdbool.h>

#ifndef BITREADER_H_INCLUDED
#define BITREADER_H_INCLUDED

struct bit_reader
{
    unsigned char *source;
    unsigned long source_length; // in bits
    unsigned long position; // in bits
    bool lock_position;

    unsigned short buffer;
    unsigned long buffer_position; // in bits
} bit_reader;

struct bit_reader new_bit_reader(unsigned char *source, unsigned long source_length);
 // only can read up to 8 bits
unsigned char read_bits(struct bit_reader *bit_reader, int bits);

#endif // BITREADER_H_INCLUDED
