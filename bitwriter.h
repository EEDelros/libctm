#include <stdbool.h>

#ifndef BITWRITER_H_INCLUDED
#define BITWRITER_H_INCLUDED

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

struct bit_writer
{
    unsigned char *target;
    unsigned long position; // in bits

    unsigned char buffer;
    unsigned char buffer_position; // in bits
} bit_writer;

struct bit_writer new_bit_writer(unsigned char *target);
// only can write up to 8 bits
void write_bits(struct bit_writer *bit_writer, unsigned char byte, unsigned char bits);
// puts into the target whats left in the buffer, then destroys the struct
void end_writer(struct bit_writer *bit_writer);

#endif // BITWRITER_H_INCLUDED
