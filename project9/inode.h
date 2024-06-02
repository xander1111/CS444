#ifndef INODE_H
#define INODE_H

#include "block.h"

#define INODE_SIZE 64
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

#define INODE_POINTER_COUNT 16
#define MAX_SYS_OPEN_FILES 64

#define TYPE_UNKNOWN 0
#define TYPE_FILE 1
#define TYPE_DIR 2

#define ROOT_INODE_NUM 0

struct inode
{
    unsigned int size;                              // offset 0
    unsigned short owner_id;                        // offset 4
    unsigned char permissions;                      // offset 6
    unsigned char flags;                            // offset 7
    unsigned char link_count;                       // offset 8
    unsigned short block_ptr[INODE_POINTER_COUNT];  // offsets 9-39 (inclusive), 2 bytes per pointer

    unsigned int ref_count;  // in-core only
    unsigned int inode_num;  // in-core only
};


struct inode *ialloc(void);

struct inode *incore_find_free(void);
struct inode *incore_find(unsigned int inode_num);

void incore_free_all(void);

void read_inode(struct inode *in, int inode_num);
void write_inode(struct inode *in);

struct inode *iget(int inode_num);
void iput(struct inode *in);

#endif
