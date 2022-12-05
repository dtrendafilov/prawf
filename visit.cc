// TODO:
// 1. instead of using list of types, we can use list of indices and get the
// types in the stub functions
// 2. benchmark against std::visit
// 3. Compare against a swiwct. How does msvc use a switch? Tail recusion
// optimization?

#include <variant>
#include <array>

struct MyVariant
{
    union {
        double as_double;
        void* as_pointer;
    } value;
};

struct Visitor
{
    void operator()(double d) const {
    }
    void operator()(void* p) const {
    }
};

namespace std
{
    template<>
    struct variant_size<MyVariant> : std::integral_constant<std::size_t, 2>
    {};
}


namespace zz
{
    template <typename... Ts>
    struct variant_types_value
    {};

    template <typename T>
    struct variant_types;

    template <typename... Ts>
    struct variant_types<std::variant<Ts...>>
    {
        typedef variant_types_value<Ts...> value;
    };

    template <>
    struct variant_types<MyVariant>
    {
        typedef variant_types_value<double, void*> value;
    };



    template <typename F, typename V, typename T>
    auto visit_stub();

    template <typename R, typename F, typename V, typename T>
    struct visit_impl;

    template <typename R, typename F, typename V, typename... Ts>
    struct visit_impl<R, F, V, variant_types_value<Ts...>>
    {
        template <typename T>
        static auto visit_stub(F& f, V& v)
        {
            return f(std::get<T>(v));
        }

        typedef R (*Stub)(F& f, V& v);
        static constexpr Stub stubs[sizeof...(Ts)] = {
            visit_stub<Ts>...
        };

        static auto invoke(F& f, V&v)
        {
            return (*stubs[v.index()])(f, v);
        }
    };

    template <typename F, typename V>
    auto visit(F f, V v)
    {
        static_assert(std::variant_size_v<V> > 0);
        typedef decltype(f(std::get<0>(v))) Result;
        typedef visit_impl<Result, F, V, typename variant_types<V>::value> impl;
        return impl::invoke(f, v);
    }

    template <typename R, typename F, typename V1, typename T1>
    struct visit_impl_2;

    template <typename R, typename F, typename V1, typename... T1s>
    struct visit_impl_2<R, F, V1, variant_types_value<T1s...>>
    {
        template <typename T1, typename T2>
        static auto visit_stub(F& f, V1& v1, V1& v2)
        {
            return f(std::get<T1>(v1), std::get<T2>(v2));
        }

        typedef R (*Stub)(F& f, V1& v1, V1& v2);
        typedef std::array<Stub, sizeof...(T1s)> Stubs1d;
        /* typedef Stub Stubs1d[sizeof...(T1s)]; */

        template <typename T1, typename... T2s>
        struct stub_helper
        {
            static constexpr Stubs1d stubs = {
                visit_stub<T1, T2s>...
            };
        };

        static constexpr Stubs1d stubs[sizeof...(T1s)] = {
            { stub_helper<T1s, T1s...>::stubs }...
        };

        static auto invoke(F& f, V1&v1, V1& v2)
        {
            return (*stubs[v1.index()][v2.index()])(f, v1, v2);
        }
    };

    template <typename F, typename V1, typename V2>
    auto visit2(F f, V1 v1, V2 v2)
    {
        typedef decltype(f(std::get<0>(v1), std::get<0>(v2))) Result;
        typedef visit_impl_2<Result, F, V1, typename variant_types<V1>::value> impl;
        return impl::invoke(f, v1, v2);
    }

    template <typename R, typename F, typename V1, typename T1, typename V2, typename T2>
    struct visit_impl_g;

    template <typename R, typename F, typename V1, typename... T1s, typename V2, typename... T2s>
    struct visit_impl_g<R, F, V1, variant_types_value<T1s...>, V2, variant_types_value<T2s...>>
    {
        template <typename T1, typename T2>
        static auto visit_stub(F& f, V1& v1, V2& v2)
        {
            return f(std::get<T1>(v1), std::get<T2>(v2));
        }

        typedef R (*Stub)(F& f, V1& v1, V2& v2);
        typedef std::array<Stub, sizeof...(T2s)> Stubs1d;
        /* typedef Stub Stubs1d[sizeof...(T1s)]; */

        template <typename T1, typename... T2sh>
        struct stub_helper
        {
            static constexpr Stubs1d stubs = {
                visit_stub<T1, T2sh>...
            };
        };

        static constexpr Stubs1d stubs[sizeof...(T1s)] = {
            { stub_helper<T1s, T2s...>::stubs }...
        };

        static auto invoke(F& f, V1&v1, V2& v2)
        {
            return (*stubs[v1.index()][v2.index()])(f, v1, v2);
        }
    };

    template <typename F, typename V1, typename V2>
    auto visitg(F f, V1 v1, V2 v2)
    {
        typedef decltype(f(std::get<0>(v1), std::get<0>(v2))) Result;
        typedef visit_impl_g<Result, F, V1, typename variant_types<V1>::value, V2, typename variant_types<V2>::value> impl;
        return impl::invoke(f, v1, v2);
    }
}

struct PrintVisitor
{
    void operator()(int x) const
    {
        std::printf("int: %d\n", x);
    }

    void operator()(double x) const
    {
        std::printf("double: %lf\n", x);
    }
};

struct SumVisitor
{
    template <typename T1, typename T2>
    double operator()(T1 v1, T2 v2) const
    {
        return v1 + v2;
    }
};

int main()
{
    MyVariant mv;
    typedef std::variant<int, double> StdV;
    StdV i{42};
    StdV d{3.14};
    zz::visit(PrintVisitor{}, i);
    zz::visit(PrintVisitor{}, d);
    typedef std::variant<double, float, int> StdV2;
    StdV2 f{3.14f};
    /* auto r = zz::visit2(SumVisitor{}, d, i); */
    auto r = zz::visitg(SumVisitor{}, d, f);
    std::printf("sum %lf\n", r);
    return 0;
}
