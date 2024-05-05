#include "block.h"

#include <unistd.h>
#include "image.h"

unsigned char *bread(int block_num, unsigned char *block)
{
    int byte_offset = block_num * BLOCK_SIZE;

    lseek(image_fd, byte_offset, SEEK_SET);
    read(image_fd, block, 4096);

    return block;
}

void bwrite(int block_num, unsigned char *block)
{
    (void)block_num;
    (void)block;
}
