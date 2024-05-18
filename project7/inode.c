#include "inode.h"

#include <stddef.h>
#include "block.h"
#include "free.h"

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

int ialloc(void)
{
    return alloc_block(INODE_FREE_BLOCK);
}

struct inode *incore_find_free(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
        if (incore[i].ref_count == 0)
            return incore + i;
    
    return NULL;
}
