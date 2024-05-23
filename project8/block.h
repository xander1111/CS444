#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_SIZE 4096
#define SUPERBLOCK 0
#define INODE_FREE_BLOCK 1
#define DATA_FREE_BLOCK 2
#define INODE_BLOCK_START 3
#define INODE_BLOCK_COUNT 4
#define FIRST_DATA_BLOCK 7

unsigned char *bread(int block_num, unsigned char *block);
void bwrite(int block_num, unsigned char *block);
int alloc(void);

#endif
