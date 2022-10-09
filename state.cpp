#include "prawf.hpp"

#include <cstdio>
#include <tuple>
#include <type_traits>
#include <vector>

namespace destruct
{
    struct init
    {
        template <typename T>
        operator T();
    };

    template <int I>
    struct order : order<I - 1>
    {};

    template <>
    struct order<0>
    {};

    template <typename T>
    constexpr auto size_(order<3>) -> decltype(T{init{}, init{}, init{}}, 0)
    {
        return 3;
    }

    template <typename T>
    constexpr auto size_(order<2>) -> decltype(T{init{}, init{}}, 0)
    {
        return 2;
    }

    template <typename T>
    constexpr auto size_(order<1>) -> decltype(T{init{}}, 0)
    {
        return 1;
    }

    template <typename T>
    constexpr auto size_(order<0>) -> decltype(T{}, 0)
    {
        return 0;
    }

    template <typename T>
    constexpr size_t size()
    {
        static_assert(std::is_aggregate_v<T>, "aggregate required");
        return size_<T>(order<3>{});
    }
}


namespace state
{

template <typename T>
auto get_state(T& value) -> typename std::enable_if<std::is_aggregate<T>::value, T>::type
{
    return value;
}

template <typename T>
auto get_state(T&v) -> typename std::enable_if<!std::is_aggregate<T>::value, decltype(v.get_state())>::type
{
    return v.get_state();
}

template <typename T>
constexpr auto get_size() -> typename std::enable_if<std::is_aggregate<T>::value, int>::type
{
    return destruct::size<T>();
}

template <typename T>
constexpr auto get_size() -> typename std::enable_if<!std::is_aggregate<T>::value, int>::type
{
    return std::tuple_size<decltype(get_state(*static_cast<T*>(nullptr)))>::value;
}



template <typename D>
struct Visitor
{
    template <typename T>
    auto VisitState(T& v) -> typename std::enable_if<get_size<T>() == 2>::type
    {
        auto [a, b] = get_state(v);
        do_visit(static_cast<D&>(*this), a);
        do_visit(static_cast<D&>(*this), b);
    }

    template <typename T>
    auto VisitState(T& v) -> typename std::enable_if<get_size<T>() == 3>::type
    {
        auto [a, b, c] = get_state(v);
        do_visit(static_cast<D&>(*this), a);
        do_visit(static_cast<D&>(*this), b);
        do_visit(static_cast<D&>(*this), c);
    }
};

template <typename V, typename T>
void do_visit(Visitor<V>& visitor, const T& value)
{
    static_cast<V&>(visitor).Visit(value);
}

}

struct PrintVisitor : state::Visitor<PrintVisitor>
{
    /* using state::Visitor<PrintVisitor>::Visit; */

    void Visit(int value)
    {
        print_indent();
        std::printf("int{%d}\n", value);
    }

    void Visit(float value)
    {
        print_indent();
        std::printf("float{%f}\n", value);
    }

    void Visit(double value)
    {
        print_indent();
        std::printf("double{%lf}\n", value);
    }

    template <typename T>
    void Visit(const T& value)
    {
        print_indent();
        std::printf("{\n");
        _indent += 4;
        VisitState(value);
        _indent -= 4;
        print_indent();
        std::printf("}\n");
    }

    int _indent = 0;
    void print_indent() {
        std::printf("%*s", _indent, "");
    }
};

namespace state
{
template <typename V, typename T, typename A>
void do_visit(Visitor<V>& visitor, const std::vector<T, A>& value)
{
    for (auto& e : value)
    {
        do_visit(static_cast<V&>(visitor), e);
    }
}
}

class A
{
public:
    A()
    {}
    A(int a, int b, double c)
        : x(a) , y(b) , z(c)
    {}

    auto get_state() const
    {
        return std::tie(x, y, z);
    }

    auto modify_state()
    {
        return std::tie(x, y, z);
    }
    int x = 42;
    int y = 6;
    double z = 3.14159;
};

class B
{
public:
    auto get_state() const
    {
        return std::tie(_a1, _a2);
    }
    A _a1;
    A _a2;
};

class C
{
public:
    auto get_state() const
    {
        return std::tie(_a1, _a2);
    }
    B _a1;
    std::vector<A> _a2 = { A{1, 2, 3}, A{4, 5, 6}};
};

TEST_CASE("State and structured binding for reflection")
{
  A a;
  static_assert(std::tuple_size<decltype(a.get_state())>::value == 3, "???");

  SUBCASE("State returns values")
  {
      auto [x, y, z] = a.get_state();
      CHECK(x == a.x);
  }

  SUBCASE("State returns references")
  {
      auto [x, y, z] = a.get_state();
      CHECK(&x == &a.x);
  }
  SUBCASE("Modified through state")
  {
      auto [x, y, z] = a.modify_state();
      x += 1;
      y *= 2;
      z *= 3;
      auto [x2, y2, z2] = a.get_state();
      CHECK(x == x2);
      CHECK(y == y2);
      CHECK(z == z2);
  }
  SUBCASE("PrintVisitor")
  {
      PrintVisitor pv;
      do_visit(pv, a);
  }
  SUBCASE("PrintVisitor More")
  {
      B b;
      std::printf("_a1 @ %p\n_a2 @ %p\n", &b._a1, &b._a2);
      PrintVisitor pv;
      do_visit(pv, b);
  }
  SUBCASE("Visit vector")
  {
      C c;
      PrintVisitor pv;
      do_visit(pv, c);
  }
}

struct Point
{
    float x;
    float y;
};

TEST_CASE("Works out of the box for structures")
{
    Point p;
    PrintVisitor pv;
    do_visit(pv, p);
}

