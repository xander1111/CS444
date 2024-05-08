#include "inode.h"

#include "block.h"
#include "free.h"

int ialloc(void)
{
    return alloc_block(INODE_FREE_BLOCK);
}
