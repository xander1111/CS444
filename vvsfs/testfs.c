#include <stdio.h>

#include "ctest.h"
#include "image.h"

#ifdef CTEST_ENABLE

int main(void)
{
    CTEST_VERBOSE(1);

    image_open("./scratch.txt", 1);

    image_close();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main(void)
{
    printf("please ensure CTEST_ENABLE is defined to run tests\n");
}

#endif
