// [delros] why does github see this as an Objective-C file? github, go home, you're drunk

#include <stdlib.h>
#include <string.h>

#include "binaryreader.h"
#include "text.h"

#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED

struct tile_data
{
    unsigned short position_1; // 2 shorts for least memory consumption while using it in the tile struct
    unsigned short position_2; // they represent the current position in a raw data
} tile_data;

struct tile
{
    unsigned short id;
    struct tile_data data;
} tile;

enum block_types
{
    BLOCK_MORPHABLE = 1, BLOCK_PORTAL = 2, BLOCK_WORLD_PORTAL = 3, BLOCK_SIGN = 4, BLOCK_LABEL = 5, BLOCK_NPC = 6
};

// [delros] lol jesus christ this look-up table is huge
const static unsigned char block_types_look_up_table[2048] = {
    [327] = BLOCK_MORPHABLE, [328] = BLOCK_MORPHABLE, [273] = BLOCK_MORPHABLE, [440] = BLOCK_MORPHABLE, [276] = BLOCK_MORPHABLE,
    [277] = BLOCK_MORPHABLE, [279] = BLOCK_MORPHABLE, [280] = BLOCK_MORPHABLE, [447] = BLOCK_MORPHABLE, [449] = BLOCK_MORPHABLE,
    [450] = BLOCK_MORPHABLE, [451] = BLOCK_MORPHABLE, [452] = BLOCK_MORPHABLE, [456] = BLOCK_MORPHABLE, [457] = BLOCK_MORPHABLE,
    [458] = BLOCK_MORPHABLE, [464] = BLOCK_MORPHABLE, [465] = BLOCK_MORPHABLE, [471] = BLOCK_MORPHABLE, [477] = BLOCK_MORPHABLE,
	[475] = BLOCK_MORPHABLE, [476] = BLOCK_MORPHABLE, [481] = BLOCK_MORPHABLE, [482] = BLOCK_MORPHABLE, [483] = BLOCK_MORPHABLE,
	[497] = BLOCK_MORPHABLE, [492] = BLOCK_MORPHABLE, [493] = BLOCK_MORPHABLE, [494] = BLOCK_MORPHABLE, [1502] = BLOCK_MORPHABLE,
	[1500] = BLOCK_MORPHABLE, [1507] = BLOCK_MORPHABLE, [1506] = BLOCK_MORPHABLE, [1581] = BLOCK_MORPHABLE, [1587] = BLOCK_MORPHABLE,
	[1588] = BLOCK_MORPHABLE, [1592] = BLOCK_MORPHABLE, [1593] = BLOCK_MORPHABLE, [1160] = BLOCK_MORPHABLE, [1594] = BLOCK_MORPHABLE,
	[1595] = BLOCK_MORPHABLE, [1597] = BLOCK_MORPHABLE, [375] = BLOCK_MORPHABLE, [376] = BLOCK_MORPHABLE, [379] = BLOCK_MORPHABLE,
	[380] = BLOCK_MORPHABLE, [377] = BLOCK_MORPHABLE, [378] = BLOCK_MORPHABLE, [438] = BLOCK_MORPHABLE, [439] = BLOCK_MORPHABLE,
	[1001] = BLOCK_MORPHABLE, [1002] = BLOCK_MORPHABLE, [1003] = BLOCK_MORPHABLE, [1004] = BLOCK_MORPHABLE, [1052] = BLOCK_MORPHABLE,
	[1053] = BLOCK_MORPHABLE, [1054] = BLOCK_MORPHABLE, [1055] = BLOCK_MORPHABLE, [1056] = BLOCK_MORPHABLE, [1092] = BLOCK_MORPHABLE,
	[275] = BLOCK_MORPHABLE, [329] = BLOCK_MORPHABLE, [338] = BLOCK_MORPHABLE, [339] = BLOCK_MORPHABLE, [340] = BLOCK_MORPHABLE,
	[448] = BLOCK_MORPHABLE, [1536] = BLOCK_MORPHABLE, [1537] = BLOCK_MORPHABLE, [1041] = BLOCK_MORPHABLE, [1042] = BLOCK_MORPHABLE,
	[1043] = BLOCK_MORPHABLE, [1075] = BLOCK_MORPHABLE, [1076] = BLOCK_MORPHABLE, [1077] = BLOCK_MORPHABLE, [1078] = BLOCK_MORPHABLE,
	[499] = BLOCK_MORPHABLE, [1116] = BLOCK_MORPHABLE, [1117] = BLOCK_MORPHABLE, [1118] = BLOCK_MORPHABLE, [1119] = BLOCK_MORPHABLE,
	[1120] = BLOCK_MORPHABLE, [1121] = BLOCK_MORPHABLE, [1122] = BLOCK_MORPHABLE, [1123] = BLOCK_MORPHABLE, [1124] = BLOCK_MORPHABLE,
	[1125] = BLOCK_MORPHABLE, [1535] = BLOCK_MORPHABLE, [1135] = BLOCK_MORPHABLE, [1134] = BLOCK_MORPHABLE, [1538] = BLOCK_MORPHABLE,
	[1140] = BLOCK_MORPHABLE, [1141] = BLOCK_MORPHABLE, [1155] = BLOCK_MORPHABLE, [1596] = BLOCK_MORPHABLE, [1605] = BLOCK_MORPHABLE,
	[1606] = BLOCK_MORPHABLE, [1607] = BLOCK_MORPHABLE, [1609] = BLOCK_MORPHABLE, [1610] = BLOCK_MORPHABLE, [1611] = BLOCK_MORPHABLE,
	[1612] = BLOCK_MORPHABLE, [1614] = BLOCK_MORPHABLE, [1615] = BLOCK_MORPHABLE, [1616] = BLOCK_MORPHABLE, [1617] = BLOCK_MORPHABLE,
    [361] = BLOCK_MORPHABLE, [1625] = BLOCK_MORPHABLE, [1627] = BLOCK_MORPHABLE, [1629] = BLOCK_MORPHABLE, [1631] = BLOCK_MORPHABLE,
    [1633] = BLOCK_MORPHABLE, [1635] = BLOCK_MORPHABLE, [1101] = BLOCK_MORPHABLE, [1102] = BLOCK_MORPHABLE, [1103] = BLOCK_MORPHABLE,
    [1104] = BLOCK_MORPHABLE, [1105] = BLOCK_MORPHABLE, [165] = BLOCK_MORPHABLE, [43] = BLOCK_MORPHABLE, [213] = BLOCK_MORPHABLE,
    [214] = BLOCK_MORPHABLE, [1011] = BLOCK_MORPHABLE, [1012] = BLOCK_MORPHABLE, [113] = BLOCK_MORPHABLE, [1619] = BLOCK_MORPHABLE,
    [184] = BLOCK_MORPHABLE, [185] = BLOCK_MORPHABLE, [467] = BLOCK_MORPHABLE, [1620] = BLOCK_MORPHABLE, [1079] = BLOCK_MORPHABLE,
    [422] = BLOCK_MORPHABLE, [461] = BLOCK_MORPHABLE, [1584] = BLOCK_MORPHABLE, [423] = BLOCK_MORPHABLE, [1027] = BLOCK_MORPHABLE,
    [1028] = BLOCK_MORPHABLE, [418] = BLOCK_MORPHABLE, [417] = BLOCK_MORPHABLE, [420] = BLOCK_MORPHABLE, [419] = BLOCK_MORPHABLE,
    [453] = BLOCK_MORPHABLE, [1517] = BLOCK_MORPHABLE, [83] = BLOCK_MORPHABLE, [77] = BLOCK_MORPHABLE, [1520] = BLOCK_MORPHABLE,
    [1080] = BLOCK_MORPHABLE, [1582] = BLOCK_MORPHABLE, [421] = BLOCK_MORPHABLE,

    [381] = BLOCK_PORTAL, [242] = BLOCK_PORTAL,

    [374] = BLOCK_WORLD_PORTAL,

    [385] = BLOCK_SIGN,

    [1000] = BLOCK_LABEL,

    [1550] = BLOCK_NPC, [1551] = BLOCK_NPC, [1552] = BLOCK_NPC, [1553] = BLOCK_NPC, [1554] = BLOCK_NPC, [1555] = BLOCK_NPC, [1556] = BLOCK_NPC,
    [1557] = BLOCK_NPC, [1558] = BLOCK_NPC, [1559] = BLOCK_NPC, [1569] = BLOCK_NPC, [1570] = BLOCK_NPC, [1571] = BLOCK_NPC, [1572] = BLOCK_NPC,
    [1573] = BLOCK_NPC, [1574] = BLOCK_NPC, [1575] = BLOCK_NPC, [1576] = BLOCK_NPC, [1577] = BLOCK_NPC, [1578] = BLOCK_NPC, [1579] = BLOCK_NPC
};

static inline unsigned char get_tile_type(unsigned short id)
{
    return block_types_look_up_table[id];
};

static inline struct tile create_block(unsigned short id)
{
    struct tile tile;
    tile.id = id;

    return tile;
};

unsigned char *get_tile_data_contents(struct tile_data *tile_data, unsigned char *raw_data);

static inline void set_tile_data_position(struct tile_data *tile_data, unsigned int position)
{
    tile_data->position_1 = position >> 16;
    tile_data->position_2 = position;
}

unsigned int get_tile_data_position(struct tile_data *tile_data);

struct tile_data get_morphable_data(struct binary_reader *reader, unsigned char *raw_data);
struct tile_data get_portal_data(struct binary_reader *reader, unsigned char *raw_data);
struct tile_data get_sign_data(struct binary_reader *reader, unsigned char *raw_data);

static inline struct tile_data get_world_portal_data(struct binary_reader *reader, unsigned char *raw_data)
{
    // [delros] a sign has the same extra bytes (string, int) as a world portal
    // 374 is the world portal's block ID in Flash Everybody Edits (not EE! nor EEU)
    return get_sign_data(reader, raw_data);
};

struct tile_data get_label_data(struct binary_reader *reader, unsigned char *raw_data);
struct tile_data get_npc_data(struct binary_reader *reader, unsigned char *raw_data);

#endif // TILE_H_INCLUDED
