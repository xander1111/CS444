#include "image.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "inode.h"
#include "block.h"
#include "dir.h"
#include "pack.h"

int image_fd = 0;

int image_open(char *filename, int truncate)
{
    if (truncate)
        image_fd = open(filename, O_RDWR|O_CREAT|O_TRUNC, 0600);
    else
        image_fd = open(filename, O_RDWR|O_CREAT, 0600);

    return image_fd;
}

int image_close(void)
{
    return close(image_fd);     
}

void mkfs(void)
{
    struct inode *inode = ialloc();
    int data_block_num = alloc() + FIRST_DATA_BLOCK;

    inode->flags = TYPE_DIR;
    inode->size = DIR_ENTRY_SIZE * 2;  // . and ..
    inode->block_ptr[0] = data_block_num;

    unsigned char block[BLOCK_SIZE];
    unsigned char *offset = block;
    char *self = ".";
    char *parent = "..";

    // Write '.' directory entry
    write_u16(offset, inode->inode_num);    
    offset += 2;

    strcpy((char*) offset, self);
    offset += 16;

    offset += DIR_ENTRY_SIZE - 2 - 16;

    // Write '..' directory entry
    write_u16(offset, inode->inode_num);
    offset += 2;

    strcpy((char*) offset, parent);

    bwrite(data_block_num, block);
    iput(inode);
}
