#include "dir.h"

#include <stdlib.h>
#include <string.h>
#include "inode.h"
#include "block.h"
#include "pack.h"

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

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    if (dir->offset >= dir->inode->size)
        return -1;  // offset is past the directory size, no more entries to read
    
    int data_block_index = dir->offset / BLOCK_SIZE;
    int offset_in_block = dir->offset % BLOCK_SIZE;
    int data_block_num = dir->inode->block_ptr[data_block_index];

    unsigned char block[BLOCK_SIZE];
    bread(data_block_num, block);

    ent->inode_num = read_u16(block + offset_in_block);
    offset_in_block += 2;

    strcpy(ent->name, (char*) block + offset_in_block);

    dir->offset += DIR_ENTRY_SIZE;

    return 0;
}

void directory_close(struct directory *dir)
{
    iput(dir->inode);
    free(dir);
}
