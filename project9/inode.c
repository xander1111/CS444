#include "inode.h"

#include <stddef.h>
#include <string.h>
#include "block.h"
#include "free.h"
#include "pack.h"

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *ialloc(void)
{
    unsigned char free_block[BLOCK_SIZE];
    bread(INODE_FREE_BLOCK, free_block);

    int first_free_index = find_free(free_block);

    if (first_free_index == -1)  
        // No free block found, don't try to write to index -1
        return NULL;

    struct inode *incore = iget(first_free_index);
    if (incore == NULL)
        return NULL;

    set_free(free_block, first_free_index, 1);
    bwrite(INODE_FREE_BLOCK, free_block);

    incore->size = 0;
    incore->owner_id = 0;
    incore->permissions = 0;
    incore->flags = 0;
    for (int i = 0; i < INODE_POINTER_COUNT; i++)
        incore->block_ptr[i] = 0;

    incore->inode_num = first_free_index;

    write_inode(incore);
    
    return incore;
}

struct inode *incore_find_free(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
        if (incore[i].ref_count == 0)
            return incore + i;
    
    return NULL;
}

struct inode *incore_find(unsigned int inode_num)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
        if (incore[i].inode_num == inode_num && incore[i].ref_count > 0)
            return incore + i;

    return NULL;
}

void incore_free_all()
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
        incore[i].ref_count = 0;
}

void read_inode(struct inode *in, int inode_num)
{
    int block_num = inode_num / INODES_PER_BLOCK + INODE_BLOCK_START;
    int inode_offset = inode_num % INODES_PER_BLOCK;
    int byte_offset = inode_offset * INODE_SIZE;

    unsigned char block_buf[BLOCK_SIZE] = {0};
    bread(block_num, block_buf);

    unsigned char *offset = block_buf + byte_offset;

    in->size = read_u32(offset);
    offset += 4;

    in->owner_id = read_u16(offset);
    offset += 2;

    in->permissions = read_u8(offset);
    offset += 1;

    in->flags = read_u8(offset);
    offset += 1;

    in->link_count = read_u8(offset);
    offset += 1;

    for (int i = 0; i < INODE_POINTER_COUNT; i++)
    {
        in->block_ptr[i] = read_u16(offset);
        offset += 2;
    }
}

void write_inode(struct inode *in)
{
    unsigned char inode_buf[INODE_SIZE] = {0};
    unsigned char *offset = inode_buf;

    write_u32(offset, in->size);
    offset += 4;

    write_u16(offset, in->owner_id);
    offset += 2;

    write_u8(offset, in->permissions);
    offset += 1;

    write_u8(offset, in->flags);
    offset += 1;

    write_u8(offset, in->link_count);
    offset += 1;

    for (int i = 0; i < INODE_POINTER_COUNT; i++)
    {
        write_u16(offset, in->block_ptr[i]);
        offset += 2;
    }

    int block_num = in->inode_num / INODES_PER_BLOCK + INODE_BLOCK_START;
    int inode_offset = in->inode_num % INODES_PER_BLOCK;
    int byte_offset = inode_offset * INODE_SIZE;

    unsigned char block_to_write[BLOCK_SIZE] = {0};
    bread(block_num, block_to_write);

    memcpy(block_to_write + byte_offset, inode_buf, INODE_SIZE);
    bwrite(block_num, block_to_write);
}

struct inode *iget(int inode_num)
{
    struct inode *incore = incore_find(inode_num);
    if (incore != NULL)
    {
        incore->ref_count++;
        return incore;
    }

    incore = incore_find_free();
    if (incore == NULL)
        return NULL;

    read_inode(incore, inode_num);
    incore->ref_count = 1;
    incore->inode_num = inode_num;

    return incore;
}

void iput(struct inode *in)
{
    if (in->ref_count == 0) return;

    in->ref_count--;

    if (in->ref_count == 0)
        write_inode(in);
}
