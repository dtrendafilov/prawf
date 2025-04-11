#include <cstdio>
#include <string>
#include <variant>

struct Vec
{
    float v[16];
};

typedef std::variant<int, double, std::string> Value;
typedef std::variant<int, double, Vec> ValuePlain;

struct Vec64
{
    alignas(64) float v[16];
};

struct Vec32
{
    alignas(32) float v[16];
};

struct Vec16
{
    alignas(16) float v[16];
};

typedef std::variant<int, double, Vec64> ValueAlign64;
typedef std::variant<int, double, Vec32> ValueAlign32;
typedef std::variant<int, double, Vec16> ValueAlign16;

union Union64 {
    int n;
    double f;
    Vec64 v;
};

int main()
{
    std::printf("variant %llu string: %llu\n", sizeof(Value), sizeof(std::string));

    std::printf("plain %llu  align %llu\n", sizeof(ValuePlain), alignof(ValuePlain));
    std::printf("aligned 16 size %llu align: %llu\n", sizeof(ValueAlign16), alignof(ValueAlign16));
    std::printf("aligned 32 size %llu align: %llu\n", sizeof(ValueAlign32), alignof(ValueAlign32));
    std::printf("aligned 64 size %llu align: %llu\n", sizeof(ValueAlign64), alignof(ValueAlign64));

    std::printf("vec plain %llu  align %llu\n", sizeof(Vec), alignof(Vec));
    std::printf("vec aligned 16 size %llu align: %llu\n", sizeof(Vec16), alignof(Vec16));
    std::printf("vec aligned 32 size %llu align: %llu\n", sizeof(Vec32), alignof(Vec32));
    std::printf("vec aligned 64 size %llu align: %llu\n", sizeof(Vec64), alignof(Vec64));

    std::printf("union aligned 64 size %llu align: %llu\n", sizeof(Union64), alignof(Union64));

    return 0;
}
