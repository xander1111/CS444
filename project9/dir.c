#include "dir.h"

#include <stdlib.h>
#include <string.h>
#include "inode.h"
#include "block.h"
#include "pack.h"
#include "dirbasename.h"

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

int directory_make(char *path)
{
    char dir_parent_path[1024];
    char dir_name[16];

    get_dirname(path, dir_parent_path);
    get_basename(path, dir_name);

    struct inode *parent_dir = namei(dir_parent_path);
    struct inode *new_dir = ialloc();
    int data_block_num = alloc() + FIRST_DATA_BLOCK;


    // Add '.' and '..' to new directory
    unsigned char dir_data[BLOCK_SIZE];
    unsigned char *offset = dir_data;
    char *self = ".";
    char *parent = "..";

    // Write '.' directory entry
    write_u16(offset, new_dir->inode_num);    
    offset += 2;

    strcpy((char*) offset, self);
    offset += 16;

    offset += DIR_ENTRY_SIZE - 2 - 16;

    // Write '..' directory entry
    write_u16(offset, parent_dir->inode_num);
    offset += 2;

    strcpy((char*) offset, parent);

    new_dir->flags = TYPE_DIR;
    new_dir->size = DIR_ENTRY_SIZE * 2;  // . and ..
    new_dir->block_ptr[0] = data_block_num;

    bwrite(data_block_num, dir_data);


    // Add entry in parent directory
    int parent_block = parent_dir->size / 4096;
    int parent_block_offset = parent_dir->size % 4096;
    unsigned char parent_data[4096];
    int parent_data_block_num = parent_dir->block_ptr[parent_block];

    if (parent_block_offset == 0)
    {
        // TODO allocate new block
        parent_data_block_num++;
    }
    else 
        bread(parent_data_block_num, parent_data);

    // Write new entry
    offset = parent_data + parent_block_offset;

    write_u16(offset, new_dir->inode_num);
    offset += 2;

    strcpy((char*) offset, dir_name);

    bwrite(parent_data_block_num, parent_data);

    parent_dir->size += DIR_ENTRY_SIZE;

    iput(new_dir);
    iput(parent_dir);

    return 0;
}

struct inode *namei(char *path)
{
    if (strcmp(path, "/") == 0)
        return iget(ROOT_INODE_NUM);

    char *token = strchr(path, '/') + 1;
    struct directory *root = directory_open(ROOT_INODE_NUM);
    struct directory_entry ent;
    while (directory_get(root, &ent) != -1)
        if (strcmp(ent.name, token) == 0)
            return iget(ent.inode_num);

    return NULL;
    // if (token == NULL)
    //     return iget(ROOT_INODE_NUM);
    // else
    //     return namei(token);
}
