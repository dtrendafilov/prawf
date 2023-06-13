#include <variant>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <format>

class JSValue;

typedef std::vector<JSValue> JSValueVector;
typedef std::unordered_map<std::string, JSValue> JSValueMap;

class JSValue
{
    typedef std::variant<bool, int, size_t, double, std::string, JSValueVector, JSValueMap> ValueType;
    ValueType m_Value;
public:
    JSValue() = default;
    JSValue(const JSValue&) = default;
    JSValue(JSValue&&) = default;

    JSValue(auto value)
        : m_Value(std::move(value))
    {}

    void set(auto value)
    {
        m_Value = std::move(value);
    }
    auto visit(const auto& v)
    {
        return std::visit(v, m_Value);
    }
    template <typename T>
    auto get() const
    {
        return std::get<T>(m_Value);
    }
};

struct PrintVisitor
{
    void operator()(auto v) const
    {
        std::cout << v << std::endl;
    }
    void operator()(int v) const
    {
        std::cout << std::format("int({})", v) << std::endl;
    }
    void operator()(const JSValueVector& v) const
    {
    }
    void operator()(const JSValueMap& v) const
    {
    }
};


int main()
{
    static_assert(sizeof(JSValue) >= 64, "how large");
    static_assert(sizeof(JSValue) <= 128, "how large");
    std::cout << sizeof(JSValue) << std::endl;
    std::cout << sizeof(JSValueVector) << std::endl;
    std::cout << sizeof(JSValueMap) << std::endl;
    std::cout << sizeof(std::string) << std::endl;
    JSValue a;
    a.set(42);
    a.visit(PrintVisitor{});
    JSValue pi(3.14);
    pi.visit(PrintVisitor{});
    JSValue s(std::string{"The answer is"});
    s.visit(PrintVisitor{});
    std::cout << pi.get<double>() << std::endl;

    return 0;
}


