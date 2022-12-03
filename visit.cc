#include <variant>

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

#if 0
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
        static constexpr Stub stubs[sizeof...(T1s)][sizeof...(T1s)] = {
            { visit_stub<T1s, T1s>... }
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
#endif
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
    typedef std::variant<double, float> StdV2;
    StdV2 f{3.14f};
    /* auto r = zz::visit2(SumVisitor{}, d, i); */
    /* std::printf("sum %lf\n", r); */
    return 0;
}
