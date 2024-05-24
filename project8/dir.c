#include "dir.h"

#include <stdlib.h>
#include "inode.h"

struct directory *directory_open(int inode_num)
{
    struct inode *inode = iget(inode_num);
    if (inode == NULL)
        return NULL;

    struct directory *dir = malloc(sizeof(struct directory));

    dir->inode = inode;
    dir->offset = 0;

    return dir;
}

