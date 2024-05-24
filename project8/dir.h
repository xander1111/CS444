#ifndef DIR_H
#define DIR_H

#include "inode.h"

#define DIR_ENTRY_SIZE 32

struct directory
{
    struct inode *inode;  // offset 0
    unsigned int offset;  // offset 2
};

struct directory_entry
{
    // TODO
};

struct directory *directory_open(int inode_num);

#endif

