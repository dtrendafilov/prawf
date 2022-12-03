#include <cstdio>
#include <variant>
#include <cassert>

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
            return mv.value.as_double;
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
            return mv.value.as_double;
        }
        if constexpr (I == 2)
        {
            return mv.value.as_pointer;
        }
    }


}

struct PrintVisitor
{
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

int main() {
    MyVariant mv;
    mv.type = MyVariantType::Number;
    mv.value.as_double = 42;

    std::printf("get: %f\n", std::get<0>(mv));

    std::printf("size: %d\n", int(std::variant_size<MyVariant>::value));
    std::printf("size: %d\n", int(std::variant_size_v<MyVariant>));
    /* std::visit(PrintVisitor{}, mv); */

    return 0;
}
