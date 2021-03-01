#include <stdbool.h>
#include "text.h"

#ifndef BINARYREADER_H_INCLUDED
#define BINARYREADER_H_INCLUDED

struct binary_reader
{
    unsigned char *source;
    int source_length;
    unsigned int position;
    bool lock_position;
} binary_reader;

struct binary_reader read_data(unsigned char *data, int length);

unsigned char read_byte(struct binary_reader *reader, int offset);
bool read_bool(struct binary_reader *reader, int offset);
unsigned short read_ushort(struct binary_reader *reader, int offset);
int read_int(struct binary_reader *reader, int offset);
unsigned int read_uint(struct binary_reader *reader, int offset);

float read_float(struct binary_reader *reader, int offset);

struct text read_text(struct binary_reader *reader, int offset);

#endif // BINARYREADER_H_INCLUDED
