#include <cstdio>
#include <cstdlib>
#include <sanitizer/asan_interface.h>

void unaddressable_write(int* p)
{
    p[4] = 2;
}

void corrupt(int* p)
{
    p[2] = 4;
    p[4] = 2;
}

int main()
{
    /* { */
    /*     auto undef = new int; */
    /*     unaddressable_write(undef); */
    /*     auto answer = new int(*undef + 42); */
    /*     std::printf("The answer is %d\n", *answer); */
    /* } */
    {
        // clang does not seem to allow this partial poison
        auto ints = new int[16]{};
        ASAN_POISON_MEMORY_REGION(ints + 4, sizeof(int));
        corrupt(ints);
        std::printf("2 -> %d 4 -> %d\n", ints[2], ints[4]);
        delete [] ints;
    }
    /* { */
    /*     auto memory = std::malloc(64); */
    /*     ASAN_POISON_MEMORY_REGION(memory, 64); */
    /*     *((int*)(memory) + 4) = 42; */
    /*     std::free(memory); */
    /* } */
    return 0;
}
