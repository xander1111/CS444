#include <stdio.h>

#include "ctest.h"
#include "image.h"

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

int main(void)
{
    CTEST_VERBOSE(1);

    test_image_open();
    test_image_close();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("please ensure CTEST_ENABLE is defined to run tests\n");
}

#endif
