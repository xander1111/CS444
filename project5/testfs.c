#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "ctest.h"
#include "image.h"
#include "block.h"


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

    unsigned char buf[4096] = {0};
    for (int i = 0; i < 4096; i++)
        buf[i] = i % 256;
    
    lseek(image_fd, 0, SEEK_SET);
    write(image_fd, buf, 4096);

    unsigned char read_data[4096] = {0};
    
    CTEST_ASSERT(bread(0, read_data) == read_data, "bread returns pointer to the buffer given");
    CTEST_ASSERT(memcmp(buf, read_data, 4096) == 0, "bread reads data into the buffer given");

    image_close();
}

void test_block_write(void)
{
    image_open("./test.txt", 1);

    unsigned char buf[4096] = {0};
    for (int i = 0; i < 4096; i++)
        buf[i] = i % 256;
       
    bwrite(0, buf);

    unsigned char read_data[4096] = {0};
    bread(0, read_data);
    CTEST_ASSERT(memcmp(buf, read_data, 4096) == 0, "bwrite writes data");

    image_close();
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_image_open();
    test_image_close();

    test_block_read();
    test_block_write();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("please ensure CTEST_ENABLE is defined to run tests\n");
}

#endif
