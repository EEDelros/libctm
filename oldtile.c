#include "binaryreader.h"
#include "tile.h"

struct tile_data get_morphable_data(struct binary_reader *reader, unsigned char* data)
{
    struct tile_data data;
    data.bytes = malloc(data.length);
    data.bytes[0] = (unsigned char) read_int(reader, 0);

    return data;
}

struct tile_data get_portal_data(struct binary_reader *reader)
{
    struct tile_data data;
    data.length = sizeof(unsigned char) + sizeof(unsigned int)*2;

    data.bytes = malloc(data.length);

    register int position = 0;

    data.bytes[position] = (unsigned char) read_int(reader, 0);
    position += sizeof(unsigned char);

    data.bytes[position] = read_int(reader, 0);
    position += sizeof(int);

    data.bytes[position] = read_int(reader, 0);

    return data;
}

struct tile_data get_sign_data(struct binary_reader *reader)
{
    struct text message = read_text(reader, 0);

    struct tile_data data;
    data.length = sizeof(unsigned char) * message.length + sizeof(unsigned char);

    data.bytes = malloc(data.length);

    register int position = 0;

    memcpy(&data.bytes[position], message.characters, sizeof(unsigned char) * message.length);
    position += sizeof(unsigned char) * message.length;

    data.bytes[position] = (unsigned char) read_int(reader, 0);

    return data;
}

struct tile_data get_label_data(struct binary_reader *reader)
{
    struct text hex_color = read_text(reader, 0);
    struct text message = read_text(reader, 0);

    struct tile_data data;
    data.length =
        sizeof(unsigned char) * hex_color.length +
        sizeof(unsigned char) * message.length +
        sizeof(int);

    data.bytes = malloc(data.length);

    register int position = 0;

    memcpy(&data.bytes[position], hex_color.characters, sizeof(unsigned char) * hex_color.length);
    position += sizeof(unsigned char) * hex_color.length;

    memcpy(&data.bytes[position], message.characters, sizeof(unsigned char) * message.length);
    position += sizeof(unsigned char) * message.length;

    data.bytes[position] = read_int(reader, 0);

    return data;
}

struct tile_data get_npc_data(struct binary_reader *reader)
{
    struct text npc_name = read_text(reader, 0);
    struct text message1 = read_text(reader, 0);
    struct text message2 = read_text(reader, 0);
    struct text message3 = read_text(reader, 0);

    struct tile_data data;
    data.length =
        sizeof(unsigned char) * npc_name.length +
        sizeof(unsigned char) * message1.length +
        sizeof(unsigned char) * message2.length +
        sizeof(unsigned char) * message3.length;

    data.bytes = malloc(data.length);

    register int position = 0;

    memcpy(&data.bytes[position], npc_name.characters, sizeof(unsigned char) * npc_name.length);
    position += sizeof(unsigned char) * npc_name.length;

    memcpy(&data.bytes[position], message1.characters, sizeof(unsigned char) * message1.length);
    position += sizeof(unsigned char) * message1.length;

    memcpy(&data.bytes[position], message2.characters, sizeof(unsigned char) * message2.length);
    position += sizeof(unsigned char) * message2.length;

    memcpy(&data.bytes[position], message3.characters, sizeof(unsigned char) * message3.length);

    return data;
}
