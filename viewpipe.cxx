#include "viewpipe.hxx"

using namespace std;

int main()
{
    /* cout << "Hello, 42" << endl; */
    auto is_odd = [](auto n) { return n % 2 != 0; };
    auto square = [](auto n) { return n * n; };
    auto square_odd = views::iota(0)
        | views::take(30)
        | views::filter(is_odd)
        | views::transform(square);
         
    for (auto v : square_odd)
    {
        cout << format("{:8}\n", v);
    }
    cout << endl;
    return 0;
}
