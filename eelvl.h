#include <stdio.h>
#include <stdbool.h>

#include "tile.h"
#include "text.h"

#ifndef EELVL_H_INCLUDED
#define EELVL_H_INCLUDED

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

typedef struct eelvl
{
    bool is_ok;
    unsigned int size;

    struct text world_name;
    struct text description;

    int width;
    int height;

    struct text owner_name;
    struct text owner_id;

    struct text crew_name;
    struct text crew_id;

    int crew_status;

    unsigned int background_color;
    float gravity;

    bool has_minimap;
    bool is_campaign;

    struct tile *tiles; // this should point to a memory allocated array
    unsigned char *raw_data; // also point to a memory allocated array
} eelvl;

extern void DLL_EXPORT dispose_eelvl(eelvl level);

extern struct eelvl DLL_EXPORT import_eelvl(char *data, int length);
extern struct eelvl DLL_EXPORT import_eelvl_file(FILE *file);
extern struct eelvl DLL_EXPORT import_eelvl_path(char *file_path);

extern unsigned char DLL_EXPORT *read_eelvl(char *data, int length, unsigned int *output_length);
extern unsigned char DLL_EXPORT *read_eelvl_file(FILE *file, unsigned int *output_length);
extern unsigned char DLL_EXPORT *read_eelvl_path(char *file_path, unsigned int *output_length);

#endif // EELVL_H_INCLUDED
