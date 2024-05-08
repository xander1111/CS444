#include "inode.h"

#include "block.h"
#include "free.h"

int ialloc(void)
{
    unsigned char free_block[4096];
    bread(INODE_FREE_BLOCK, free_block);

    int first_free_index = find_free(free_block);

    set_free(free_block, first_free_index, 1);

    bwrite(INODE_FREE_BLOCK, free_block);

    return first_free_index;
}

int alloc(void)
{
    return 0;
}
