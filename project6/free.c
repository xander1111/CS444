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

int find_free(unsigned char *block)
{
    (void)block;
    return 0;
}
