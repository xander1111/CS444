#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "ctest.h"
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "dir.h"
#include "ls.h"


#ifdef CTEST_ENABLE

#define EXAMPLE_INODE_NUM 3

unsigned char example_inode_block[BLOCK_SIZE];  // Has some inode data at EXAMPLE_INODE_NUM

void generate_example_inode_block(void)
{    
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
    memcpy(example_inode_block + EXAMPLE_INODE_NUM * INODE_SIZE, i, 41);
}

// Example incore inode contains the same data as in the example inode block
struct inode *get_example_incore_inode(void)
{
    struct inode *incore = incore_find_free();
    incore->size = 0xAABBCCDD;
    incore->owner_id = 0x1A1B;
    incore->permissions = 0x10;
    incore->flags = 0xAB;
    incore->link_count = 0x20;

    for (int i = 0; i < INODE_POINTER_COUNT; i++)
        incore->block_ptr[i] = 0x0101 * (i + 1);
    
    incore->ref_count = 1;
    incore->inode_num = EXAMPLE_INODE_NUM;
    return incore;
}

void setup_inode_file(void)
{    
    bwrite(INODE_BLOCK_START, example_inode_block);
}

void fill_incore_inodes(void)
{
    struct inode *inode;
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
    {
        inode = incore_find_free();
        inode->ref_count = 1;
    }
}

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

    int bit_num = 123;
    unsigned char block[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        block[i] = 0xFF;

    set_free(block, bit_num, 0);
    bwrite(INODE_FREE_BLOCK, block);  // inode free block gets set to all 1s, except for bit 123

    CTEST_ASSERT(ialloc()->inode_num = EXAMPLE_INODE_NUM, "ialloc allocates the first free inode");

    image_close();
}

void test_ialloc_fail(void)
{
    image_open("./test.txt", 1);

    unsigned char block[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; i++)
        block[i] = 0xFF;

    bwrite(INODE_FREE_BLOCK, block);  // inode free block gets set to all 1s

    CTEST_ASSERT(ialloc() == NULL, "ialloc returns NULL if there are no free inodes");

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
    struct inode *inode = incore_find_free();

    CTEST_ASSERT(inode->ref_count == 0, "incore_find_free returns an inode that is not in use");

    incore_free_all();
}

void test_incore_find_free_null(void)
{
    fill_incore_inodes();

    CTEST_ASSERT(incore_find_free() == NULL, "incore_find_free returns NULL if no inodes are free");
    incore_free_all();
}

void test_incore_find(void)
{
    struct inode *inode = incore_find_free();
    inode->inode_num = 1;
    inode->ref_count = 1;

    inode = incore_find_free();
    inode->inode_num = 2;
    inode->ref_count = 1;

    CTEST_ASSERT(incore_find(1)->inode_num == 1, "incore_find returns the incore inode with the given inode number");
    CTEST_ASSERT(incore_find(2)->inode_num == 2, "incore_find returns the incore inode with the given inode number");

    incore_free_all();
}

void test_incore_find_null(void)
{
    CTEST_ASSERT(incore_find(1) == NULL, "incore_find returns NULL if the given inode number is not in memory");
    incore_free_all();
}

void test_write_inode(void)
{
    image_open("./test.txt", 1);

    struct inode *incore = get_example_incore_inode();

    write_inode(incore);

    unsigned char read_data[BLOCK_SIZE] = {0};
    bread(INODE_BLOCK_START, read_data);

    CTEST_ASSERT(memcmp(example_inode_block, read_data, BLOCK_SIZE) == 0, "write_inode can write an inode to disk");

    incore_free_all();
    image_close();
}

void test_read_inode(void)
{
    image_open("./test.txt", 1);

    setup_inode_file();

    struct inode to_read;
    read_inode(&to_read, EXAMPLE_INODE_NUM);

    unsigned char has_same_data = to_read.size == 2864434397 &&  // 0xAABBCCDD
    to_read.owner_id == 6683 &&  // 0x1A1B
    to_read.permissions == 0x10 &&
    to_read.flags == 0xAB &&
    to_read.link_count == 0x20;

    for (int i = 0; i < INODE_POINTER_COUNT; i++)
        has_same_data &= to_read.block_ptr[i] == 0x0101 * (i + 1);

    CTEST_ASSERT(has_same_data, "read_inode can be used to read inodes from disk");

    incore_free_all();
    image_close();
}

void test_iget(void)
{
    image_open("./test.txt", 1);

    setup_inode_file();

    struct inode *to_read;

    to_read = iget(EXAMPLE_INODE_NUM);

    unsigned char has_same_data = to_read->size == 2864434397 &&  // 0xAABBCCDD
    to_read->owner_id == 6683 &&  // 0x1A1B
    to_read->permissions == 0x10 &&
    to_read->flags == 0xAB &&
    to_read->link_count == 0x20 &&
    to_read->inode_num == EXAMPLE_INODE_NUM;

    for (int i = 0; i < INODE_POINTER_COUNT; i++)
        has_same_data &= to_read->block_ptr[i] == 0x0101 * (i + 1);
    
    CTEST_ASSERT(has_same_data, "iget read the inode from disk and returns an incore inode");

    CTEST_ASSERT(iget(EXAMPLE_INODE_NUM) == to_read, "iget returns the existing incore inode if it's already in memory");

    incore_free_all();
    image_close();
}

void test_iget_null(void)
{
    image_open("./test.txt", 1);

    fill_incore_inodes();

    CTEST_ASSERT(iget(1) == NULL, "iget returns NULL if there are no free incore inodes");

    incore_free_all();
}

void test_iput(void)
{
    image_open("./test.txt", 1);

    struct inode *incore = get_example_incore_inode();
    iput(incore);

    CTEST_ASSERT(incore->ref_count == 0, "iput decrements the reference count of an inode");

    unsigned char read_data[BLOCK_SIZE] = {0};
    bread(INODE_BLOCK_START, read_data);

    CTEST_ASSERT(memcmp(example_inode_block, read_data, BLOCK_SIZE) == 0, "iput writes the inode to disk if it gets freed");

    incore_free_all();
    image_close();
}

void test_iput_already_free(void)
{
    image_open("./test.txt", 1);

    struct inode *incore = get_example_incore_inode();
    incore->ref_count = 0;
    iput(incore);

    CTEST_ASSERT(incore->ref_count == 0, "iput doesn't decrement the reference count of an inode if it's already 0");

    incore_free_all();
    image_close();
}

void test_mkfs(void)
{
    image_open("./test.txt", 1);

    mkfs();

    struct inode *incore = iget(ROOT_INODE_NUM);
    CTEST_ASSERT(incore->flags == 2, "mkfs creates a directory at inode_num 0");

    unsigned char data[BLOCK_SIZE];
    bread(incore->block_ptr[0], data);

    unsigned char has_same_data = data[0] == 0x00 && data[1] == 0x00 &&  // dir entry 0 inode_num
    data[2] == '.' &&  // filename

    data[0 + DIR_ENTRY_SIZE] == 0x00 && data[1 + DIR_ENTRY_SIZE] == 0x00 &&  // dir entry 1 inode_num
    data[2 + DIR_ENTRY_SIZE] == '.' && data[3 + DIR_ENTRY_SIZE] == '.';  //filename

    CTEST_ASSERT(has_same_data, "mkfs creates a root directory with itself as . and ..");

    incore_free_all();
    image_close();
}

void test_dir_open(void)
{
    image_open("./test.txt", 1);
    mkfs();

    struct directory *root = directory_open(ROOT_INODE_NUM);

    CTEST_ASSERT(root->inode != NULL, "directory_open returns a directory with an inode");
    CTEST_ASSERT(root->offset == 0, "directory_open has a default offset of 0");

    incore_free_all();
    image_close();
}

void test_dir_open_null(void)
{
    image_open("./test.txt", 1);
    mkfs();

    fill_incore_inodes();

    struct directory *root = directory_open(10);

    CTEST_ASSERT(root == NULL, "directory_open returns NULL when an error occurs");

    incore_free_all();
    image_close();
}

void test_dir_get(void)
{
    image_open("./test.txt", 1);
    mkfs();

    struct directory *root = directory_open(ROOT_INODE_NUM);
    
    struct directory_entry entry = {};

    int err = directory_get(root, &entry);
    char entry1;
    strcpy(&entry1, entry.name);

    CTEST_ASSERT(err == 0 && entry.name[0] != 0, "directory_get provides directory entries");
    
    err = directory_get(root, &entry);
    CTEST_ASSERT(err == 0 && strcmp(entry.name, &entry1) != 0, "directory_get provides different entries when repeatedly called");

    err = directory_get(root, &entry);
    CTEST_ASSERT(err == -1, "directory_get returns -1 when there are no more entries to read");

    incore_free_all();
    image_close();
}

void test_dir_close(void)
{
    image_open("./test.txt", 1);

    struct directory *root = directory_open(ROOT_INODE_NUM);

    int inode_num = root->inode->inode_num;
    root->inode->size = 1;

    directory_close(root);

    incore_free_all();  // Force iget() to get the inode from disk
    struct inode *incore = iget(inode_num);
    CTEST_ASSERT(incore->size == 1, "directory_close updates the directory's inode on disk");

    incore_free_all();
    image_close();
}

void test_namei(void)
{
    image_open("./test.txt", 1);
    mkfs();

    CTEST_ASSERT(namei("/")->inode_num == 0, "namei can get the root directory");

    incore_free_all();
    image_close();
}

void test_namei_1_layer(void)
{
    image_open("./test.txt", 1);
    mkfs();
    directory_make("/foo");

    struct directory *root = directory_open(ROOT_INODE_NUM);
    struct directory_entry entry;

    while (directory_get(root, &entry) != -1);

    CTEST_ASSERT(namei("/foo")->inode_num == entry.inode_num, "namei can find a directory one level down");

    incore_free_all();
    image_close();
}

void test_namei_multi_layer(void)
{
    image_open("./test.txt", 1);
    mkfs();
    directory_make("/foo");
    directory_make("/foo/bar");
    directory_make("/foo/bar/baz");

    struct directory *parent = directory_open(ROOT_INODE_NUM);
    struct directory_entry entry;

    while (directory_get(parent, &entry) != -1);
    parent = directory_open(entry.inode_num);  // new directory '/foo' should be the last directory entry

    while (directory_get(parent, &entry) != -1);
    parent = directory_open(entry.inode_num);  // new directory '/foo/bar' should be the last directory entry

    while (directory_get(parent, &entry) != -1);  // new directory '/foo/bar/baz' should be the last directory entry

    CTEST_ASSERT(namei("/foo/bar/baz")->inode_num == entry.inode_num, "namei can find a directory multiple levels down");

    incore_free_all();
    image_close();
}

void test_namei_invalid(void)
{
    image_open("./test.txt", 1);
    mkfs();

    CTEST_ASSERT(namei("/foo") == NULL, "namei returns NULL when given a filepath that does not exist");

    incore_free_all();
    image_close();
}

void test_dir_make(void)
{
    image_open("./test.txt", 1);
    mkfs();

    directory_make("/foo");

    struct directory *root = directory_open(ROOT_INODE_NUM);
    struct directory_entry entry;

    while (directory_get(root, &entry) != -1);

    CTEST_ASSERT(strcmp(entry.name, "foo") == 0, "directory_make creates a new directory and adds an entry for it to the root directory");

    incore_free_all();
    image_close();
}

void example_ls(void)
{
    image_open("./test.txt", 1);
    mkfs();

    directory_make("/foo");
    directory_make("/bar");
    directory_make("/foo/baz");

    puts("\nExample ls:");
    ls(ROOT_INODE_NUM);
    puts("\nls in /foo:");
    ls(1);

    incore_free_all();
    image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

    generate_example_inode_block();

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

    test_iput();
    test_iput_already_free();

    test_mkfs();

    test_dir_open();
    test_dir_open_null();

    test_dir_get();

    test_dir_close();

    test_namei();
    test_namei_1_layer();
    test_namei_multi_layer();
    test_namei_invalid();

    test_dir_make();

    example_ls();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("please ensure CTEST_ENABLE is defined to run tests\n");
}

#endif
