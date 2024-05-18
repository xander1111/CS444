#ifndef FREE_H
#define FREE_H

void set_free(unsigned char *block, int num, int set);
int find_free(unsigned char *block);
int alloc_free(int block_index);

#endif
