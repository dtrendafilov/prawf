#include <cstdio>
#include <string>
#include <variant>

typedef std::variant<int, double, std::string> Value;
typedef std::variant<int, double> ValuePlain;

int main()
{
    std::printf("plain %llu\n", sizeof(ValuePlain));
    std::printf("variant %llu\nstring: %llu\n", sizeof(Value),
            sizeof(std::string));
    return 0;
}
