#include <cstdio>
#include <cstdlib>
// It seems that the annotations are disabled for clang, but clang-cl is OK
#include <drmemory_annotations.h>

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
    {
        auto undef = new int;
        unaddressable_write(undef);
        auto answer = new int(*undef + 42);
        std::printf("The answer is %d\n", *answer);
    }
    {
        auto ints = new int[16];
        DRMEMORY_ANNOTATE_MAKE_UNADDRESSABLE(ints + 4, sizeof(int));
        corrupt(ints);
        delete [] ints;
    }
    {
        auto memory = std::malloc(64);
        DRMEMORY_ANNOTATE_MAKE_UNADDRESSABLE(memory, 64);
        *((int*)(memory) + 4) = 42;
        std::free(memory);
    }
    return 0;
}
