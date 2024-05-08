#include "free.h"

#include "block.h"

void set_free(unsigned char *block, int num, int set)
{
    int byte = num / 8;
    int bit = num % 8;

    if (set)
        block[byte] |= 1 << bit;
    else
        block[byte] &= ~(1 << bit);
}

int find_clear_bit(unsigned char byte)
{
    for (int i = 0; i < 8; i++)
        if (!(byte & (1 << i)))
            return i;

    return -1;
}

int find_free(unsigned char *block)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
        if (block[i] != 0xFF)
            return i * 8 + find_clear_bit(block[i]);
    
    return -1;
}
