#ifndef INODE_H
#define INODE_H

#include "block.h"

#define INODE_SIZE 64
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

#define INODE_POINTER_COUNT 16
#define MAX_SYS_OPEN_FILES 64

struct inode
{
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_POINTER_COUNT];

    unsigned int ref_count;  // in-core only
    unsigned int inode_num;  // in-core only
};


int ialloc(void);
struct inode *incore_find_free(void);

#endif
