#include "block.h"

#include <unistd.h>
#include "image.h"
#include "free.h"

unsigned char *bread(int block_num, unsigned char *block)
{
    int byte_offset = block_num * BLOCK_SIZE;

    lseek(image_fd, byte_offset, SEEK_SET);
    read(image_fd, block, BLOCK_SIZE);

    return block;
}

void bwrite(int block_num, unsigned char *block)
{
    int byte_offset = block_num * BLOCK_SIZE;

    lseek(image_fd, byte_offset, SEEK_SET);
    write(image_fd, block, BLOCK_SIZE);
}

int alloc(void)
{
    return alloc_block(DATA_FREE_BLOCK);
}
