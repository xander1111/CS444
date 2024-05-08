#ifndef FREE_H
#define FREE_H

void set_free(unsigned char *block, int num, int set);
int find_free(unsigned char *block);
int alloc_block(int block_index);

#endif
