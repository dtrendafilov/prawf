#include <cstdio>

template <int... Ns>
struct Ints
{
};

struct S
{
    int x;
    int y;
};

template <int N, typename T>
struct Array1;

template <int N, int... Ns>
struct Array1<N, Ints<Ns...>>
{
    static constexpr S array[sizeof...(Ns)] = {
        S{N, Ns}...
    };
};

template <typename T1, typename T2>
struct Array;

template <int... N1s, int... N2s>
struct Array<Ints<N1s...>, Ints<N2s...>>
{
    typedef const S SubArray[sizeof...(N2s)];
    static constexpr SubArray array[sizeof...(N1s)] = {
        {Array1<N1s, Ints<N2s...>>::array}...
    };
    static void dump()
    {
        for (auto i = 0u; i < sizeof...(N1s); ++i)
        {
            for (auto j = 0u; j < sizeof...(N2s); ++j)
            {
                std::printf("{%d, %d} ", array[i][j].x, array[i][j].y);
            }
            std::printf("\n");
        }
    }
};


int main()
{
    typedef Array<Ints<0, 1, 2>, Ints<2, 3, 4, 5>> a;
    a::dump();
    return 0;
}
