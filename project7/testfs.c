#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "ctest.h"
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"


#ifdef CTEST_ENABLE

void test_image_open(void)
{
    CTEST_ASSERT(image_open("./test.txt", 1) > 0, "image_open returns a file descriptor"); 
    CTEST_ASSERT(image_fd != 0, "image fd gets set on open");
}

void test_image_close(void)
{
    CTEST_ASSERT(image_close() == 0, "image_close succeeds");
}

void test_block_read(void)
{
    image_open("./test.txt", 1);

    unsigned char buf[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        buf[i] = i % 256;
    
    lseek(image_fd, 0, SEEK_SET);
    write(image_fd, buf, BLOCK_SIZE);

    unsigned char read_data[BLOCK_SIZE] = {0};
    
    CTEST_ASSERT(bread(0, read_data) == read_data, "bread returns pointer to the buffer given");
    CTEST_ASSERT(memcmp(buf, read_data, BLOCK_SIZE) == 0, "bread reads data into the buffer given");

    image_close();
}

void test_block_write(void)
{
    image_open("./test.txt", 1);

    unsigned char buf[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        buf[i] = i % 256;
       
    bwrite(0, buf);

    unsigned char read_data[BLOCK_SIZE] = {0};
    bread(0, read_data);
    CTEST_ASSERT(memcmp(buf, read_data, BLOCK_SIZE) == 0, "bwrite writes data");

    image_close();
}

void test_set_free(void)
{
    int bit_num = 100;
    int byte = bit_num / 8;
    int bit_in_byte = bit_num % 8;
    
    unsigned char block[BLOCK_SIZE] = {0};

    set_free(block, bit_num, 1);

    CTEST_ASSERT((block[byte] & 1 << bit_in_byte) >> bit_in_byte, "set_free can set a bit to 1 at the specified location");
}

void test_set_free_to_0(void)
{
    int bit_num = 100;
    int byte = bit_num / 8;
    int bit_in_byte = bit_num % 8;
    
    unsigned char block[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        block[i] = 0xFF;

    set_free(block, bit_num, 0);

    CTEST_ASSERT(!((block[byte] & 1 << bit_in_byte) >> bit_in_byte), "set_free can set a bit to 0 at the specified location");
}

void test_find_free(void)
{
    int bit_num = 123;
    
    unsigned char block[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        block[i] = 0xFF;

    set_free(block, bit_num, 0);

    CTEST_ASSERT(find_free(block) == bit_num, "find_free returns the index of the first 0 bit");
}

void test_ialloc(void)
{
    image_open("./test.txt", 1);

    int bit_num = 321;
    unsigned char block[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        block[i] = 0xFF;

    set_free(block, bit_num, 0);
    bwrite(INODE_FREE_BLOCK, block);  // inode free block gets set to all 1s, except for bit 321

    CTEST_ASSERT(ialloc() == bit_num, "ialloc allocates the first free inode");

    image_close();
}

void test_ialloc_fail(void)
{
    image_open("./test.txt", 1);

    unsigned char block[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        block[i] = 0xFF;

    bwrite(INODE_FREE_BLOCK, block);  // inode free block gets set to all 1s

    CTEST_ASSERT(ialloc() == -1, "ialloc returns -1 if there are no free inodes");

    image_close();
}

void test_alloc(void)
{
    image_open("./test.txt", 1);

    int bit_num = 321;
    unsigned char block[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        block[i] = 0xFF;

    set_free(block, bit_num, 0);
    bwrite(DATA_FREE_BLOCK, block);  // data free block gets set to all 1s, except for bit 321

    CTEST_ASSERT(alloc() == bit_num, "alloc allocates the first free data block");

    image_close();
}

void test_alloc_fail(void)
{
    image_open("./test.txt", 1);

    unsigned char block[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        block[i] = 0xFF;

    bwrite(DATA_FREE_BLOCK, block);  // data free block gets set to all 1s

    CTEST_ASSERT(alloc() == -1, "alloc returns -1 if there are no free data blocks");

    image_close();
}

void test_incore_find_free(void)
{
    incore_free_all();
    struct inode *inode = incore_find_free();

    CTEST_ASSERT(inode->ref_count == 0, "incore_find_free returns an inode that is not in use");
}

void test_incore_find_free_null(void)
{
    incore_free_all();
    struct inode *inode = incore_find_free();

    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        inode = incore_find_free();
        inode->ref_count = 1;
    }

    CTEST_ASSERT(incore_find_free() == NULL, "incore_find_free returns NULL if no inodes are free");
}

void test_incore_find(void)
{
    incore_free_all();
    struct inode *inode = incore_find_free();
    inode->inode_num = 1;
    inode->ref_count = 1;

    inode = incore_find_free();
    inode->inode_num = 2;
    inode->ref_count = 1;

    CTEST_ASSERT(incore_find(1)->inode_num == 1, "incore_find returns the incore inode with the given inode number");
    CTEST_ASSERT(incore_find(2)->inode_num == 2, "incore_find returns the incore inode with the given inode number");
}

void test_incore_find_null(void)
{
    incore_free_all();
    CTEST_ASSERT(incore_find(1) == NULL, "incore_find returns NULL if the given inode number is not in memory");
}

void test_write_inode(void)
{
    image_open("./test.txt", 1);
    incore_free_all();

    unsigned char block_buf[BLOCK_SIZE] = {0};
    
    unsigned char i[] = {
        0xAA, 0xBB, 0xCC, 0xDD,     // size
        0x1A, 0x1B,                 // owner_id
        0x10,                       // permissions
        0xAB,                       // flags
        0x20,                       // link_count
        0x01, 0x01,                 // block pointers
        0x02, 0x02, 
        0x03, 0x03,
        0x04, 0x04,
        0x05, 0x05,
        0x06, 0x06,
        0x07, 0x07,
        0x08, 0x08,
        0x09, 0x09,
        0x0A, 0x0A,
        0x0B, 0x0B,
        0x0C, 0x0C,
        0x0D, 0x0D,
        0x0E, 0x0E,
        0x0F, 0x0F,
        0x10, 0x10
    };
    memcpy(block_buf + 3 * INODE_SIZE, i, 41);  // only 41 of the allocated 64 bytes are used

    struct inode *incore = incore_find_free();
    incore->size = 0xAABBCCDD;
    incore->owner_id = 0x1A1B;
    incore->permissions = 0x10;
    incore->flags = 0xAB;
    incore->link_count = 0x20;

    for (int i = 0; i < INODE_POINTER_COUNT; i++)
        incore->block_ptr[i] = 0x0101 * (i + 1);
    
    incore->ref_count = 0;
    incore->inode_num = 3;

    write_inode(incore);

    unsigned char read_data[BLOCK_SIZE] = {0};
    bread(INODE_BLOCK_START, read_data);

    CTEST_ASSERT(memcmp(block_buf, read_data, BLOCK_SIZE) == 0, "write_inode can write an inode to disk");
}

void test_read_inode(void)
{
    image_open("./test.txt", 1);
    incore_free_all();

    unsigned char block_buf[BLOCK_SIZE] = {0};
    
    unsigned char i[] = {
        0xAA, 0xBB, 0xCC, 0xDD,     // size
        0x1A, 0x1B,                 // owner_id
        0x10,                       // permissions
        0xAB,                       // flags
        0x20,                       // link_count
        0x01, 0x01,                 // block pointers
        0x02, 0x02, 
        0x03, 0x03,
        0x04, 0x04,
        0x05, 0x05,
        0x06, 0x06,
        0x07, 0x07,
        0x08, 0x08,
        0x09, 0x09,
        0x0A, 0x0A,
        0x0B, 0x0B,
        0x0C, 0x0C,
        0x0D, 0x0D,
        0x0E, 0x0E,
        0x0F, 0x0F,
        0x10, 0x10
    };
    memcpy(block_buf + 3 * INODE_SIZE, i, 41);  // only 41 of the allocated 64 bytes are used
    bwrite(INODE_BLOCK_START, block_buf);

    struct inode to_read;
    read_inode(&to_read, 3);

    unsigned char has_same_data = to_read.size == 2864434397 &&  // 0xAABBCCDD
    to_read.owner_id == 6683 &&  // 0x1A1B
    to_read.permissions == 0x10 &&
    to_read.flags == 0xAB &&
    to_read.link_count == 0x20;

    for (int i = 0; i < INODE_POINTER_COUNT; i++)
        has_same_data &= to_read.block_ptr[i] == 0x0101 * (i + 1);

    CTEST_ASSERT(has_same_data, "read_inode can be used to read inodes from disk");
}

void test_iget(void)
{
    image_open("./test.txt", 1);
    incore_free_all();

    unsigned char block_buf[BLOCK_SIZE] = {0};
    
    unsigned char i[] = {
        0xAA, 0xBB, 0xCC, 0xDD,     // size
        0x1A, 0x1B,                 // owner_id
        0x10,                       // permissions
        0xAB,                       // flags
        0x20,                       // link_count
        0x01, 0x01,                 // block pointers
        0x02, 0x02, 
        0x03, 0x03,
        0x04, 0x04,
        0x05, 0x05,
        0x06, 0x06,
        0x07, 0x07,
        0x08, 0x08,
        0x09, 0x09,
        0x0A, 0x0A,
        0x0B, 0x0B,
        0x0C, 0x0C,
        0x0D, 0x0D,
        0x0E, 0x0E,
        0x0F, 0x0F,
        0x10, 0x10
    };
    memcpy(block_buf + 3 * INODE_SIZE, i, 41);  // only 41 of the allocated 64 bytes are used
    bwrite(INODE_BLOCK_START, block_buf);

    struct inode *to_read;

    to_read = iget(3);

    unsigned char has_same_data = to_read->size == 2864434397 &&  // 0xAABBCCDD
    to_read->owner_id == 6683 &&  // 0x1A1B
    to_read->permissions == 0x10 &&
    to_read->flags == 0xAB &&
    to_read->link_count == 0x20 &&
    to_read->inode_num == 3;

    for (int i = 0; i < INODE_POINTER_COUNT; i++)
        has_same_data &= to_read->block_ptr[i] == 0x0101 * (i + 1);
    
    CTEST_ASSERT(has_same_data, "iget reads and returns an incore inode");

    CTEST_ASSERT(iget(3) == to_read, "iget returns the existing incore inode if it's already in memory");
}

void test_iget_null(void)
{
    image_open("./test.txt", 1);
    incore_free_all();

    struct inode *inode;
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        inode = incore_find_free();
        inode->ref_count = 1;
    }

    CTEST_ASSERT(iget(1) == NULL, "iget returns NULL if there are no free incore inodes");
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_image_open();
    test_image_close();

    test_block_read();
    test_block_write();

    test_set_free();
    test_set_free_to_0();
    test_find_free();

    test_ialloc();
    test_ialloc_fail();

    test_alloc();
    test_alloc_fail();

    test_incore_find_free();
    test_incore_find_free_null();

    test_incore_find();
    test_incore_find_null();

    test_write_inode();
    test_read_inode();

    test_iget();
    test_iget_null();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("please ensure CTEST_ENABLE is defined to run tests\n");
}

#endif
