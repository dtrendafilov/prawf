#include <cstdio>

void custom_assert(bool value, const char* expression)
{
    if (!value)
    {
        fprintf(stderr, "failed assert: (%s)\n", expression);
    }
}

#define ZZ_ASSERT(E) custom_assert(E, #E);

int main(int argc, const char* argv[])
{
    ZZ_ASSERT(argc > 1);
    return 0;
}
