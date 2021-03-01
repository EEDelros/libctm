#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "binaryreader.h"

struct binary_reader read_data(unsigned char *data, int length)
{
    struct binary_reader new_reader;

    new_reader.source = data;
    new_reader.source_length = length;
    new_reader.position = 0;
    new_reader.lock_position = false;

    return new_reader;
}

unsigned char read_byte(struct binary_reader *reader, int offset)
{
    unsigned char byte = reader->source[reader->position + offset];
    if (reader->lock_position == false)
        reader->position += sizeof(unsigned char);
    return byte;
}

bool read_bool(struct binary_reader *reader, int offset)
{
    unsigned char byte = reader->source[reader->position + offset];
    if (reader->lock_position == false)
        reader->position += sizeof(unsigned char);
    if (byte > 0)
        return true;
    else
        return false;
}

unsigned short read_ushort(struct binary_reader *reader, int offset)
{
    register int type_size = sizeof(unsigned short);

    unsigned short unsigned_short = 0;
    for (register int i = 0; i < type_size; i++)
        unsigned_short |= (reader->source[reader->position + offset + i] << ((type_size - 1) - i)*8);
    if (reader->lock_position == false)
        reader->position += type_size;

    return unsigned_short;
}

int read_int(struct binary_reader *reader, int offset)
{
    register int type_size = sizeof(int);

    register int integer = 0;
    for (register int i = 0; i < type_size; i++)
        integer |= (reader->source[reader->position + offset + i] << ((type_size - 1) - i)*8);
    if (reader->lock_position == false)
        reader->position += type_size;

    return integer;
}

unsigned int read_uint(struct binary_reader *reader, int offset)
{
    return read_int(reader, offset);
}

float read_float(struct binary_reader *reader, int offset)
{
    int single = read_int(reader, offset);
    return *(float *) &single;
    // [delros] MWAHAHAHAHAHAHAHA THANKS QUAKE
    // hopefully i dont get screamed at by a real C programmer hahahaha, unless? 0_0
}

struct text read_text(struct binary_reader *reader, int offset)
{
    unsigned short length = read_ushort(reader, offset);
    // strings have a unsigned short as their length in actionscript 3

    unsigned char *characters = calloc(length, sizeof(unsigned char));
    memcpy(characters, &reader->source[reader->position + offset], sizeof(unsigned char)*length);

    struct text text;
    text.characters = characters;
    text.length = length;

    if (reader->lock_position == false)
        reader->position += sizeof(unsigned char)*length;

    return text;
}
