#include <zlib.h>
#include <stdlib.h>
#include <stdbool.h>

#include "eelvl.h"
#include "binaryreader.h"
#include "tile.h"

extern void DLL_EXPORT dispose_eelvl(eelvl level)
{
    // [delros] No, this doesn't cause a memory leak. Too good!
    free(level.owner_name.characters);
    free(level.world_name.characters);
    free(level.description.characters);
    free(level.crew_id.characters);
    free(level.crew_name.characters);
    free(level.owner_id.characters);
    free(level.tiles);
    free(level.raw_data);
}

extern struct eelvl DLL_EXPORT import_eelvl(char *data, int length)
{
    unsigned int output_length = 0;
    unsigned char *output = read_eelvl(data, length, &output_length);

    struct binary_reader reader = read_data(output, output_length);

    struct eelvl level;

    // read the header part of our eelvl data
    level.owner_name = read_text(&reader, 0);
    level.world_name = read_text(&reader, 0);
    level.width = read_int(&reader, 0);
    level.height = read_int(&reader, 0);
    level.gravity = read_float(&reader, 0);
    level.background_color = read_uint(&reader, 0);
    level.description = read_text(&reader, 0);
    level.is_campaign = read_bool(&reader, 0);
    level.crew_id = read_text(&reader, 0);
    level.crew_name = read_text(&reader, 0);
    level.crew_status = read_int(&reader, 0);
    level.has_minimap = read_bool(&reader, 0);
    level.owner_id = read_text(&reader, 0);
    level.size = output_length;

    // data's header was read successfully, now we read the blocks stored in our eelvl's data
    level.tiles = calloc(level.width*level.height*2, sizeof(tile));

    register int background_position = level.width*level.height;

    register unsigned short block_id, layer = 0;
    struct tile_data tile_data;
    while (reader.position < reader.source_length)
    {
        block_id = (unsigned short) read_int(&reader, 0);
        layer = (unsigned short) read_int(&reader, 0);

        register unsigned int array_length = read_uint(&reader, 0);
        register unsigned int half_array_length = array_length/2; // [delros] divisions are expensive DUH

        // [delros] why can't ee's eelvl parser just use a freaking array of shorts??
        // worlds would be lighter that way, jesus hahhaha this is hilarious

        register int tile_type = get_tile_type(block_id);
        register unsigned int seek_ahead = 0;
        if (tile_type > 0)
        {
            register unsigned int original_reader_position = reader.position;

            reader.position += sizeof(unsigned int) + sizeof(unsigned short)*array_length;
            register unsigned int data_start_position = reader.position;

            if (tile_type == BLOCK_MORPHABLE)
                tile_data = get_morphable_data(&reader, output);
            else if (tile_type == BLOCK_PORTAL)
                tile_data = get_portal_data(&reader, output);
            else if (tile_type == BLOCK_WORLD_PORTAL)
                tile_data = get_world_portal_data(&reader, output);
            else if (tile_type == BLOCK_SIGN)
                tile_data = get_sign_data(&reader, output);
            else if (tile_type == BLOCK_LABEL)
                tile_data = get_label_data(&reader, output);
            else if (tile_type == BLOCK_NPC)
                tile_data = get_npc_data(&reader, output);

            seek_ahead = reader.position - data_start_position;

            reader.position = original_reader_position;
        }
        else
        {
            tile_data.position_1 = 0;
            tile_data.position_2 = 0;
        }

        reader.lock_position = true; // [delros] so read calls don't advance our reader's position
        for (register int i = 0; i < half_array_length; i++)
        {
            register unsigned short x = read_ushort(&reader, sizeof(unsigned short) * i);
            register unsigned short y = read_ushort(&reader,
                sizeof(unsigned short) * (half_array_length + i) + sizeof(unsigned int));

            register struct tile block = create_block(block_id);
            block.data = tile_data;

            level.tiles[x + y*level.width + background_position*layer] = block;
        }
        reader.lock_position = false;
        reader.position += sizeof(unsigned short)*array_length + sizeof(unsigned int) + seek_ahead;
    }
    // [delros] the "register" keyword abuse is real LOL

    level.raw_data = output;
    level.is_ok = true;

    return level;
}

extern struct eelvl DLL_EXPORT import_eelvl_file(FILE *file)
{
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char buffer[size];
    fread(buffer, size, 1, file);

    return import_eelvl(buffer, size);
}

extern struct eelvl DLL_EXPORT import_eelvl_path(char *file_path)
{
    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        struct eelvl level;
        level.is_ok = false;
        return level;
    }

    struct eelvl level = import_eelvl_file(file);
    fclose(file);

    return level;
}

extern unsigned char DLL_EXPORT *read_eelvl(char *data, int length, unsigned int *output_length)
{
    int err = 0;

    z_stream stream;

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    err = inflateInit2(&stream, -15);
    if (err != Z_OK)
    {
        printf("Error: inflateInit %d\n", err);
    }

    stream.avail_in = length;
    stream.next_in = (Bytef *)data;

    unsigned int out_size = 16384;
    unsigned char *bytes = calloc(out_size, sizeof(unsigned char));
    stream.next_out = bytes;
    stream.avail_out = out_size;
    do {
        err = inflate(&stream, Z_NO_FLUSH);
        if (stream.avail_out != 0)
            break;
        if (stream.total_out >= out_size)
        {
            stream.avail_out = out_size;
            bytes = realloc(bytes, out_size*sizeof(unsigned char)*2);
            stream.next_out = &bytes[out_size];
            out_size *= 2;
        }
    } while (err != Z_STREAM_END);

    // stream.total_out = bytes gotten from decompression
    *output_length = stream.total_out;
    bytes = realloc(bytes, stream.total_out);

    inflateEnd(&stream);

    return bytes;
}

extern unsigned char DLL_EXPORT *read_eelvl_file(FILE *file, unsigned int *output_length)
{
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char buffer[size];
    fread(buffer, size, 1, file);

    return read_eelvl(buffer, size, output_length);
}

extern unsigned char DLL_EXPORT *read_eelvl_path(char *file_path, unsigned int *output_length)
{
    FILE *file = fopen(file_path, "rb");
    if (!file)
        return (unsigned char *)"file not found";

    unsigned char *output = read_eelvl_file(file, output_length);
    fclose(file);

    return output;
}
