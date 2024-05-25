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
    unsigned int inode_num;
    char name[16];
};

struct directory *directory_open(int inode_num);
int directory_get(struct directory *dir, struct directory_entry *ent);

#endif

