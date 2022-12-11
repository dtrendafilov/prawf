// TODO:
// 1. benchmark against std::visit
// 2. Compare against a switch. How does msvc use a switch? Tail recusion
// optimization?

#include <variant>
#include <array>
#include <cassert>
#include <utility>


enum class MyVariantType
{
    Number,
    Boolean,
    Pointer,
};

struct MyVariant
{
    MyVariantType type;
    union {
        double as_double;
        bool as_bool;
        void* as_pointer;
    } value;

    // std::variant interface
    constexpr std::size_t index() const noexcept
    {
        return std::size_t(type);
    }

    constexpr bool valueless_by_exception() const noexcept
    {
        return false;
    }
};

namespace std
{
    template <typename T>
    constexpr bool holds_alternative(const MyVariant& mv)
    {
        return false;
    }
    template <>
    constexpr bool holds_alternative<double>(const MyVariant& mv)
    {
        return mv.index() == 0;
    }

    template <>
    constexpr bool holds_alternative<bool>(const MyVariant& mv)
    {
        return mv.index() == 1;
    }
    
    template <>
    constexpr bool holds_alternative<void*>(const MyVariant& mv)
    {
        return mv.index() == 2;
    }

    template <>
    struct variant_size<MyVariant> : std::integral_constant<std::size_t, 3>
    {} ;

    template <>
    struct variant_alternative<0, MyVariant>
    {
        typedef double type;
    };

    template <>
    struct variant_alternative<1, MyVariant>
    {
        typedef bool type;
    };

    template <>
    struct variant_alternative<2, MyVariant>
    {
        typedef void* type;
    };


    template <size_t I>
    constexpr auto get(const MyVariant& mv) -> const variant_alternative_t<I, MyVariant>&
    {
        assert(I == mv.index());
        if constexpr (I == 0)
        {
            return mv.value.as_double;
        }
        if constexpr (I == 1)
        {
            return mv.value.as_double;
        }
        if constexpr (I == 2)
        {
            return mv.value.as_pointer;
        }
    }

    template <size_t I>
    constexpr auto get(MyVariant& mv) -> variant_alternative_t<I, MyVariant>&
    {
        assert(I == mv.index());
        if constexpr (I == 0)
        {
            return mv.value.as_double;
        }
        if constexpr (I == 1)
        {
            return mv.value.as_bool;
        }
        if constexpr (I == 2)
        {
            return mv.value.as_pointer;
        }
    }
    template <size_t I>
    constexpr auto get(MyVariant&& mv) -> variant_alternative_t<I, MyVariant>&&
    {
        assert(I == mv.index());
        if constexpr (I == 0)
        {
            return mv.value.as_double;
        }
        if constexpr (I == 1)
        {
            return mv.value.as_bool;
        }
        if constexpr (I == 2)
        {
            return mv.value.as_pointer;
        }
    }


}

struct PrintVisitor
{
    void operator()(int d)
    {
        std::printf("%d\n", d);
    }
    void operator()(double d)
    {
        std::printf("%lf\n", d);
    }
    void operator()(bool b)
    {
        std::printf("%d\n", int(b));
    }
    void operator()(void* p)
    {
        std::printf("%p\n", p);
    }
};


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
        typedef variant_types_value<double, bool, void*> value;
    };


    template <typename F, typename V, typename T>
    auto visit_stub();

    template <typename R, typename F, typename V, typename T>
    struct visit_impl;

    template <typename R, typename F, typename V, int... Is>
    struct visit_impl<R, F, V, std::integer_sequence<int, Is...>>
    {
        template <int I>
        static auto visit_stub(F& f, V& v)
        {
            return f(std::get<I>(v));
        }

        typedef R (*Stub)(F& f, V& v);
        static constexpr Stub stubs[sizeof...(Is)] = {
            visit_stub<Is>...
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
        typedef decltype(std::make_integer_sequence<int, std::variant_size_v<V>>()) Indices;
        typedef visit_impl<Result, F, V, Indices> impl;
        return impl::invoke(f, v);
    }

    template <typename R, typename F, typename V1, typename T1, typename V2, typename T2>
    struct visit_impl_g;

    template <typename R, typename F, typename V1, int... I1s, typename V2, int... I2s>
    struct visit_impl_g<R, F, V1, std::integer_sequence<int, I1s...>, V2, std::integer_sequence<int, I2s...>>
    {
        template <int I1, int I2>
        static auto visit_stub(F& f, V1& v1, V2& v2)
        {
            return f(std::get<I1>(v1), std::get<I2>(v2));
        }

        typedef R (*Stub)(F& f, V1& v1, V2& v2);
        typedef std::array<Stub, sizeof...(I2s)> Stubs1d;
        /* typedef Stub Stubs1d[sizeof...(T1s)]; */

        template <int I1, int... I2sh>
        struct stub_helper
        {
            static constexpr Stubs1d stubs = {
                visit_stub<I1, I2sh>...
            };
        };

        static constexpr Stubs1d stubs[sizeof...(I1s)] = {
            { stub_helper<I1s, I2s...>::stubs }...
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
        typedef decltype(std::make_integer_sequence<int, std::variant_size_v<V1>>()) Indices1;
        typedef decltype(std::make_integer_sequence<int, std::variant_size_v<V2>>()) Indices2;
        typedef visit_impl_g<Result, F, V1, Indices1, V2, Indices2> impl;
        return impl::invoke(f, v1, v2);
    }
}

struct SumVisitor
{
    template <typename T1, typename T2>
    double operator()(T1 v1, T2 v2) const
    {
        assert(false);
        return 0;
    }

    double operator()(double v1, double v2) const
    {
        return v1 + v2;
    }

    double operator()(double v1, float v2) const
    {
        return v1 + v2;
    }
};

int main()
{
    MyVariant mv;
    mv.type = MyVariantType::Number;
    mv.value.as_double = 1234.5678;
    zz::visit(PrintVisitor{}, mv);
    MyVariant mv2;
    mv2.type = MyVariantType::Number;
    mv2.value.as_double = 8765.4321;
    std::printf("%lf\n", zz::visitg(SumVisitor{}, mv, mv2));


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
