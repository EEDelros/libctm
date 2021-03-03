#include <stdbool.h>
#include "bitreader.h"
#include "bitwriter.h"
#include "eelvl.h"

#define BLOCK_SIZE 16
#define BLOCK_SIZE_POWER_OF_TWO BLOCK_SIZE*BLOCK_SIZE

#ifndef CTM_H_INCLUDED
#define CTM_H_INCLUDED

struct block_line
{
    unsigned char position;
    unsigned char length;
    bool vertical;
    unsigned char id;
} block_line;

struct block_rectangle
{
    unsigned char position;
    unsigned char size;
    unsigned char id;
} block_rectangle;

struct block_chunk
{
    struct block_rectangle *block_rectangles;
    unsigned short block_rectangle_count;

    struct block_line *block_lines;
    unsigned short block_line_count;

    unsigned short *different_block_ids;
    unsigned short different_block_count;
} block_chunk;

const static unsigned char look_up_byte_bit_usage[256] = {
    [0 ... 1] = 1,
    [2 ... 3] = 2,
    [4 ... 7] = 3,
    [8 ... 15] = 4,
    [16 ... 31] = 5,
    [32 ... 63] = 6,
    [64 ... 127] = 7,
    [128 ... 255] = 8
};

void DLL_EXPORT eelvl_to_ctm(struct eelvl *level);

#endif // CTM_H_INCLUDED
