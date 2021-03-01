#include "binaryreader.h"
#include "tile.h"

struct tile_data get_morphable_data(struct binary_reader *reader, unsigned char *raw_data)
{
    struct tile_data data;
    register int length = sizeof(unsigned int);
    set_tile_data_position(&data, reader->position);
    //data.bytes = &raw_data[reader->position];

    reader->position += length;

    return data;
}

struct tile_data get_portal_data(struct binary_reader *reader, unsigned char *raw_data)
{
    struct tile_data data;
    register int length = sizeof(unsigned int)*3;
    set_tile_data_position(&data, reader->position);
    //data.bytes = &raw_data[reader->position];

    reader->position += length;

    return data;
}

struct tile_data get_sign_data(struct binary_reader *reader, unsigned char *raw_data)
{
    struct tile_data data;
    register int length = 0;
    set_tile_data_position(&data, reader->position);
    //data.bytes = &raw_data[reader->position];

    length += sizeof(unsigned short) + read_ushort(reader, 0) + sizeof(int);
    reader->position += length - sizeof(unsigned short);

    return data;
}

struct tile_data get_label_data(struct binary_reader *reader, unsigned char *raw_data)
{
    struct tile_data data;
    set_tile_data_position(&data, reader->position);
    //data.bytes = &raw_data[reader->position];

    register unsigned short text_length = 0;
    for (register int i = 0; i < 2; i++)
    {
        text_length = read_ushort(reader, 0);
        reader->position += text_length;
    }

    reader->position += sizeof(int);

    return data;
}

struct tile_data get_npc_data(struct binary_reader *reader, unsigned char *raw_data)
{
    struct tile_data data;
    set_tile_data_position(&data, reader->position);
    //data.bytes = &raw_data[reader->position];

    register unsigned short text_length = 0;
    for (register int i = 0; i < 4; i++)
    {
        text_length = read_ushort(reader, 0);
        reader->position += text_length;
    }

    return data;
}

unsigned int get_tile_data_position(struct tile_data *tile_data)
{
    unsigned int value = (unsigned int)tile_data->position_1 << 16
        | (unsigned int)tile_data->position_2;

    return value;
}

unsigned char *get_tile_data_contents(struct tile_data *tile_data, unsigned char *raw_data)
{
    return &raw_data[get_tile_data_position(tile_data)];
}
