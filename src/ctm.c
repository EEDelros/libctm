#include <stdbool.h>
#include <stdlib.h>
#include "ctm.h"

unsigned short min(unsigned short a, unsigned short b)
{
    if (a < b)
        return a;
    else
        return b;
}

unsigned short max(unsigned short a, unsigned short b)
{
    if (a > b)
        return a;
    else
        return b;
}

static inline unsigned char get_bit_usage(unsigned short n)
{
    if (n < 256)
        return look_up_byte_bit_usage[n];
    else
        return 8 + look_up_byte_bit_usage[(n >> 8) & 0x00FF];
}

short *get_block_chunks_id_array(struct block_chunk *block_chunk)
{
    short *ids = calloc(BLOCK_SIZE_POWER_OF_TWO, sizeof(short));

    unsigned short current_position = 0;
    for (register int i = 0; i < block_chunk->block_rectangle_count; i++)
    {
        struct block_rectangle *block_rectangle = &block_chunk->block_rectangles[i];

        unsigned char x = current_position & 0x0F;
        unsigned char y = (current_position & 0xF0) >> 4;

        unsigned char width = 1 + (block_rectangle->size & 0x0F);
        unsigned char height = 1 + ((block_rectangle->size & 0xF0) >> 4);

        for (register int y1 = y; y1 < y + height; y1++)
        {
            for (register int x1 = x; x1 < x + width; x1++)
            {
                unsigned char id = block_rectangle->id;
                ids[x1 + y1*BLOCK_SIZE] = id + 1;
            }
        }

        if (width >= BLOCK_SIZE)
            current_position += height * BLOCK_SIZE;
        else
            current_position += width;

        while (ids[current_position] != 0)
            current_position++;
    }

    return ids;
}

void compress_to_block_rectangles(struct block_chunk *block_chunk)
{
    unsigned char block_chunk_bytes[BLOCK_SIZE_POWER_OF_TWO*4 + 1];
    struct bit_writer bit_writer = new_bit_writer(block_chunk_bytes);
    register int i = 0;

    write_bits(&bit_writer, 0b00000000, 1);
    write_bits(&bit_writer, 0b00000000, 2);

    unsigned char look_up_table_size = block_chunk->different_block_count - 1;
    int length_bit_usage = get_bit_usage(look_up_table_size);
    write_bits(&bit_writer, look_up_table_size, 8);

    // GET DELTA ID WITH MOST BIT-CONSUMPTION
    unsigned short biggest_delta_id = 0;
    unsigned short last_id = 0;
    for (register int i = 0; i < block_chunk->different_block_count; i++)
    {
        unsigned short id = block_chunk->different_block_ids[i];
        biggest_delta_id = max(biggest_delta_id, id - last_id);
        last_id = id;
    }

    int biggest_delta_id_bit_usage = get_bit_usage(biggest_delta_id);
    unsigned short x = min(8, biggest_delta_id_bit_usage);

    // write the bit consumption of any block id in our block chunk's block dictionary
    write_bits(&bit_writer, biggest_delta_id_bit_usage, 4);

    // WRITE ALL IDS
    last_id = 0;
    for (i = 0; i < block_chunk->different_block_count; i++)
    {
        unsigned short id = block_chunk->different_block_ids[i];

        write_bits(&bit_writer, (id - last_id) << (8 - x), x);
        if (biggest_delta_id_bit_usage > 8)
            write_bits(&bit_writer, (id - last_id) << (biggest_delta_id_bit_usage - 8), biggest_delta_id_bit_usage - 8);

        last_id = id;
    }

    // WRITE THE CONTENTS
    write_bits(&bit_writer, block_chunk->block_rectangle_count - 1, 8);
    for (i = 0; i < block_chunk->block_rectangle_count; i++)
    {
        struct block_rectangle *block_rectangle = &block_chunk->block_rectangles[i];

        unsigned char width_usage = get_bit_usage(15 - (block_rectangle->position & 0x0F));
        unsigned char height_usage = get_bit_usage(15 - ((block_rectangle->position & 0xF0) >> 4));

        write_bits(&bit_writer, (block_rectangle->size & 0x0F) << (8 - width_usage), width_usage);
        write_bits(&bit_writer, (block_rectangle->size & 0xF0) << (4 - height_usage), height_usage);

        write_bits(&bit_writer, block_rectangle->id << (8 - length_bit_usage), length_bit_usage);
    }

    // were done writing data
    int data_size = bit_writer.position >> 3;
    if (bit_writer.buffer_position > 0) data_size++;
    end_writer(&bit_writer);

    // print our DEETA
    printf("16x16 block chunk bytes: %u\n", data_size);
    printf("data: \n");
    for (register int i = 0; i < data_size; i++)
        printf("%c", block_chunk_bytes[i]);
    printf("\n\n");
}

void compress_to_block_array(struct block_chunk *block_chunk)
{
    unsigned char block_chunk_bytes[BLOCK_SIZE_POWER_OF_TWO*4 + 1];
    struct bit_writer bit_writer = new_bit_writer(block_chunk_bytes);
    register int i = 0;

    write_bits(&bit_writer, 0b00000000, 1);
    write_bits(&bit_writer, 0b01000000, 2);

    unsigned char look_up_table_size = block_chunk->different_block_count - 1;
    int length_bit_usage = get_bit_usage(look_up_table_size);
    write_bits(&bit_writer, look_up_table_size, 8);

    // GET DELTA ID WITH MOST BIT-CONSUMPTION
    unsigned short biggest_delta_id = 0;
    unsigned short last_id = 0;
    for (register int i = 0; i < block_chunk->different_block_count; i++)
    {
        unsigned short id = block_chunk->different_block_ids[i];
        biggest_delta_id = max(biggest_delta_id, id - last_id);
        last_id = id;
    }

    int biggest_delta_id_bit_usage = get_bit_usage(biggest_delta_id);
    unsigned short x = min(8, biggest_delta_id_bit_usage);

    // write the bit consumption of any block id in our block chunk's block dictionary
    write_bits(&bit_writer, biggest_delta_id_bit_usage, 4);

    // WRITE ALL IDS
    last_id = 0;
    for (i = 0; i < block_chunk->different_block_count; i++)
    {
        unsigned short id = block_chunk->different_block_ids[i];

        write_bits(&bit_writer, (id - last_id) << (8 - x), x);
        if (biggest_delta_id_bit_usage > 8)
            write_bits(&bit_writer, (id - last_id) << (biggest_delta_id_bit_usage - 8), biggest_delta_id_bit_usage - 8);

        last_id = id;
    }

    // WRITE THE CONTENTS
    short *ids = get_block_chunks_id_array(block_chunk);
    for (i = 0; i < BLOCK_SIZE_POWER_OF_TWO; i++)
    {
        unsigned short id = ids[i] - 1;
        write_bits(&bit_writer, id << (8 - length_bit_usage), length_bit_usage);
    }
    free(ids);

    // were done writing data
    int data_size = bit_writer.position >> 3;
    if (bit_writer.buffer_position > 0) data_size++;
    end_writer(&bit_writer);

    // print our DEETA
    printf("16x16 block chunk bytes: %u\n", data_size);
    printf("data: \n");
    for (register int i = 0; i < data_size; i++)
        printf("%c", block_chunk_bytes[i]);
    printf("\n\n");
}

void compress_to_filled_chunk(struct block_chunk *block_chunk)
{
    unsigned char block_chunk_bytes[16];
    struct bit_writer bit_writer = new_bit_writer(block_chunk_bytes);

    write_bits(&bit_writer, 0b00000000, 1);
    write_bits(&bit_writer, 0b10000000, 2);

    int biggest_id_bit_usage = get_bit_usage(block_chunk->different_block_ids[0]);
    write_bits(&bit_writer, biggest_id_bit_usage, 4);

    unsigned short id = block_chunk->different_block_ids[0];
    unsigned short capped_bit_usage = min(8, biggest_id_bit_usage);
    write_bits(&bit_writer, id << (8 - capped_bit_usage), capped_bit_usage);
    if (biggest_id_bit_usage > 8)
        write_bits(&bit_writer, id << (biggest_id_bit_usage - 8), biggest_id_bit_usage - 8);

    // set size to 0 for now
    write_bits(&bit_writer, 0, 8);

    // were done writing data
    int data_size = bit_writer.position >> 3;
    if (bit_writer.buffer_position > 0) data_size++;
    end_writer(&bit_writer);

    // print our DEETA
    printf("16x16 block chunk bytes: %u\n", data_size);
    printf("data: \n");
    for (register int i = 0; i < data_size; i++)
        printf("%c", block_chunk_bytes[i]);
    printf("\n\n");
}

void compress_to_id_array(struct block_chunk *block_chunk)
{
    unsigned char block_chunk_bytes[BLOCK_SIZE_POWER_OF_TWO*2 + 1];
    struct bit_writer bit_writer = new_bit_writer(block_chunk_bytes);
    register int i = 0;

    write_bits(&bit_writer, 0b00000000, 1);
    write_bits(&bit_writer, 0b11000000, 2);

    unsigned short biggest_id = 0;
    for (i = 0; i < block_chunk->different_block_count; i++)
        biggest_id = max(biggest_id, block_chunk->different_block_ids[i]);

    int biggest_id_bit_usage = get_bit_usage(biggest_id);
    write_bits(&bit_writer, biggest_id_bit_usage, 4);

    short *ids = get_block_chunks_id_array(block_chunk);
    for (i = 0; i < BLOCK_SIZE_POWER_OF_TWO; i++)
    {
        unsigned short id = ids[i] - 1;
        unsigned short capped_bit_usage = min(8, biggest_id_bit_usage);
        write_bits(&bit_writer, id << (8 - capped_bit_usage), capped_bit_usage);
        if (biggest_id_bit_usage > 8)
            write_bits(&bit_writer, id << (biggest_id_bit_usage - 8), biggest_id_bit_usage - 8);
    }
    free(ids);

    // were done writing data
    int data_size = bit_writer.position >> 3;
    if (bit_writer.buffer_position > 0) data_size++;
    end_writer(&bit_writer);

    // print our DEETA
    printf("16x16 block chunk bytes: %u\n", data_size);
    printf("data: \n");
    for (register int i = 0; i < data_size; i++)
        printf("%c", block_chunk_bytes[i]);
    printf("\n\n");
}

void encode(struct block_chunk *block_chunk)
{
    unsigned int length_bit_usage = get_bit_usage(block_chunk->different_block_count - 1);

    unsigned short biggest_delta_id = 0;
    unsigned short last_id = 0;
    for (register int i = 0; i < block_chunk->different_block_count; i++)
    {
        unsigned short id = block_chunk->different_block_ids[i];
        biggest_delta_id = max(biggest_delta_id, id - last_id);
        last_id = id;
    }

    unsigned short biggest_id = 0;
    for (register int i = 0; i < block_chunk->different_block_count; i++)
        biggest_id = max(biggest_id, block_chunk->different_block_ids[i]);

    unsigned int biggest_id_bit_usage = get_bit_usage(biggest_id);
    unsigned int biggest_delta_id_bit_usage = get_bit_usage(biggest_delta_id);

    unsigned int block_rectangle_chunk_bound =
        3 + 8 + 4 + biggest_delta_id_bit_usage*block_chunk->different_block_count + 8;

    for (register int i = 0; i < block_chunk->block_rectangle_count; i++)
    {
        struct block_rectangle *block_rectangle = &block_chunk->block_rectangles[i];

        unsigned char x = block_rectangle->position & 0x0F;
        unsigned char y = (block_rectangle->position & 0xF0) >> 4;
        unsigned char width_usage = get_bit_usage(15 - x);
        unsigned char height_usage = get_bit_usage(15 - y);

        printf("rect size usage: %u\n", width_usage + height_usage);
        block_rectangle_chunk_bound += width_usage + height_usage + length_bit_usage;
    }

    unsigned int block_array_chunk_bound =
        3 + 8 + 4 + biggest_delta_id_bit_usage*block_chunk->different_block_count
        + length_bit_usage*BLOCK_SIZE_POWER_OF_TWO;

    unsigned int id_array_chunk_bound = 3 + biggest_id_bit_usage*BLOCK_SIZE_POWER_OF_TWO;

    printf("predicted bit usage (block rectangle compression with info discard): %u\n", block_rectangle_chunk_bound);
    printf("predicted bit usage (block array compression): %u\n", block_array_chunk_bound);
    printf("predicted bit usage (id array compression): %u\n", id_array_chunk_bound);

    printf("\n");
    if (block_rectangle_chunk_bound > id_array_chunk_bound &&
        block_array_chunk_bound > id_array_chunk_bound)
    {
        printf("selected compression: id array chunk\n\n");
        compress_to_id_array(block_chunk);
    }
    else if (block_rectangle_chunk_bound > block_array_chunk_bound)
    {
        printf("selected compression: block array chunk\n\n");
        compress_to_block_array(block_chunk);
    }
    else if (block_chunk->different_block_count == 1)
    {
        printf("selected compression: block filled chunk\n\n");
        compress_to_filled_chunk(block_chunk);
    }
    else
    {
        printf("selected compression: block rectangle chunk\n\n");
        compress_to_block_rectangles(block_chunk);
    }
}

int compare(const void *a, const void *b)
{
    // used for sorting block ids, or just numbers in general
    return (*(unsigned short *)a - *(unsigned short *)b);
}

void DLL_EXPORT eelvl_to_ctm(struct eelvl *level)
{
    struct block_chunk block_chunk;
    block_chunk.block_rectangles = calloc(BLOCK_SIZE_POWER_OF_TWO, sizeof(block_rectangle));
    block_chunk.different_block_ids = calloc(BLOCK_SIZE_POWER_OF_TWO, sizeof(unsigned short));
    block_chunk.block_rectangle_count = 0;
    block_chunk.different_block_count = 0;

    struct block_rectangle current_block_rectangle;

    int *block_rectangle_pointers = calloc(BLOCK_SIZE_POWER_OF_TWO, sizeof(int));

    unsigned short last_id = 0;
    unsigned char dictionary_position = 0;

    for (register int y = 0; y < BLOCK_SIZE; y++)
    {
        for (register int x = 0; x < BLOCK_SIZE; x++)
        {
            struct tile block = level->tiles[x + y*level->width];

            // OPTIMIZATION #1
            bool add_block_to_different_block_list = false;
            if (last_id != block.id || (x == 0 && y == 0))
            {
                add_block_to_different_block_list = true;
                for (register int i = 0; i < block_chunk.different_block_count; i++)
                {
                    if (block.id == block_chunk.different_block_ids[i])
                    {
                        add_block_to_different_block_list = false;
                        dictionary_position = i;
                        break;
                    }
                }
            }
            if (add_block_to_different_block_list == true)
            {
                block_chunk.different_block_ids[block_chunk.different_block_count] = block.id;
                dictionary_position = block_chunk.different_block_count;
                block_chunk.different_block_count++;
            }

            // OPTIMIZATION #2
            if (last_id != block.id || x == 0 || x == BLOCK_SIZE - 1)
            {
                if (last_id == block.id)
                    current_block_rectangle.size++;

                bool add_block_rectangle = true;
                if (x > 0)
                {
                    if (y > 0)
                    {
                        int point_to = block_rectangle_pointers[current_block_rectangle.position - BLOCK_SIZE] - 1;
                        if (point_to >= 0)
                        {
                            struct block_rectangle *above_block_rectangle = &block_chunk.block_rectangles[point_to];

                            // get the width of the above block rectangle, ranges from 0 to 15
                            unsigned char above_rectangle_width = above_block_rectangle->size & 0xF;
                            if (current_block_rectangle.id == above_block_rectangle->id
                                && (current_block_rectangle.size & 0xF) == above_rectangle_width)
                            {
                                add_block_rectangle = false;
                                above_block_rectangle->size += 1 << 4; // 1 << 4 also means "16",
                                block_rectangle_pointers[current_block_rectangle.position] = point_to + 1;
                            }
                        }
                    }

                    if (add_block_rectangle == true)
                    {
                        block_chunk.block_rectangles[block_chunk.block_rectangle_count] = current_block_rectangle;
                        block_rectangle_pointers[current_block_rectangle.position] = block_chunk.block_rectangle_count + 1;
                        block_chunk.block_rectangle_count++;
                    }
                }
                if (last_id != block.id && x == BLOCK_SIZE - 1)
                {
                    struct block_rectangle *skipped_block_rectangle =
                        &block_chunk.block_rectangles[block_chunk.block_rectangle_count];

                    unsigned char skipped_position = current_block_rectangle.position +
                        (current_block_rectangle.size & 0x0F) + 1;

                    int point_to = -1;
                    if (y > 0)
                        point_to = block_rectangle_pointers[skipped_position - BLOCK_SIZE] - 1;

                    bool m = true;
                    if (point_to >= 0)
                    {
                        struct block_rectangle *above_block_rectangle = &block_chunk.block_rectangles[point_to];

                        if (dictionary_position == above_block_rectangle->id)
                        {
                            // make the above block taller
                            above_block_rectangle->size += 1 << 4; // 1 << 4 also means "16",
                            block_rectangle_pointers[skipped_position] = point_to + 1;

                            m = false;
                        }
                    }
                    if (m == true)
                    {
                        // CREATE A NEW BLOCK
                        skipped_block_rectangle->id = dictionary_position;
                        skipped_block_rectangle->position = skipped_position;
                        skipped_block_rectangle->size = 0;
                        skipped_block_rectangle->initialized = true;

                        block_rectangle_pointers[skipped_position] = block_chunk.block_rectangle_count + 1;
                        printf("block added at: %u,%u\n",
                               skipped_position & 0x0F, (skipped_position & 0xF0) >> 4);

                        block_chunk.block_rectangle_count++;
                    }
                }

                current_block_rectangle.id = dictionary_position;
                current_block_rectangle.position = x + y*BLOCK_SIZE;
                current_block_rectangle.size = 0;
                current_block_rectangle.initialized = true;
            }
            else
            {
                current_block_rectangle.size++;
            }

            last_id = block.id;
        }
    }
    free(block_rectangle_pointers);

    // we need to sort the block dictionary so that delta encoding works
    unsigned short unsorted_block_ids[BLOCK_SIZE_POWER_OF_TWO];
    memcpy(unsorted_block_ids, block_chunk.different_block_ids,
           sizeof(unsigned short) * block_chunk.different_block_count);

    // sort the block chunk's block ids
    qsort(block_chunk.different_block_ids, block_chunk.different_block_count,
          sizeof(unsigned short), compare);

    // update block rect's id to the new positions
    for (register int i = 0; i < block_chunk.block_rectangle_count; i++)
    {
        struct block_rectangle *block_rectangle = &block_chunk.block_rectangles[i];
        unsigned short id = unsorted_block_ids[block_rectangle->id];

        unsigned short *location;
        location = (unsigned short*) bsearch(&id, block_chunk.different_block_ids,
            block_chunk.different_block_count, sizeof(unsigned short), compare);

        block_rectangle->id = location - block_chunk.different_block_ids;
    }

    encode(&block_chunk);

    // [delros] print the result, this also counts as a part of decoding a block chunk
    short *ids = calloc(BLOCK_SIZE_POWER_OF_TWO, sizeof(unsigned short));

    unsigned short current_position = 0;
    for (register int i = 0; i < block_chunk.block_rectangle_count; i++)
    {
        struct block_rectangle *block_rectangle = &block_chunk.block_rectangles[i];

        unsigned char x = current_position & 0x0F;
        unsigned char y = (current_position & 0xF0) >> 4;

        unsigned short width = 1 + (block_rectangle->size & 0x0F);
        unsigned short height = 1 + ((block_rectangle->size & 0xF0) >> 4);

        //printf("blockrect | pos: %u,%u | size: %u,%u | id: %c\n", x, y, width - 1, height - 1, i + 49);

        for (register int y1 = y; y1 < y + height; y1++)
        {
            for (register int x1 = x; x1 < x + width; x1++)
            {
                unsigned char id = block_rectangle->id;
                ids[x1 + y1*BLOCK_SIZE] = i + 1;//id + 1;
            }
        }

        if (width >= BLOCK_SIZE)
            current_position += height * BLOCK_SIZE;
        else
            current_position += width;

        while (ids[current_position] != 0)
            current_position++;
    }

    for (register int y = 0; y < BLOCK_SIZE; y++)
    {
        printf("\n");
        for (register int x = 0; x < BLOCK_SIZE; x++)
        {
            unsigned char id = ids[x + y*BLOCK_SIZE] - 1;
            /*if (id == 1)
                printf(" ");
            else
                printf("#");*/
            printf("%c", id + 49);
        }
    }
    printf("\n\n");
    free(ids);

    free(block_chunk.block_rectangles);
    free(block_chunk.different_block_ids);
}