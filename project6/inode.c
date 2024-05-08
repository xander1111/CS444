#include "inode.h"

#include "block.h"
#include "free.h"

int alloc_block(int block_index)
{
    unsigned char free_block[BLOCK_SIZE];
    bread(block_index, free_block);

    int first_free_index = find_free(free_block);

    set_free(free_block, first_free_index, 1);

    bwrite(block_index, free_block);

    return first_free_index;
}

int ialloc(void)
{
    return alloc_block(INODE_FREE_BLOCK);
}

int alloc(void)
{
    return alloc_block(DATA_FREE_BLOCK);
}
