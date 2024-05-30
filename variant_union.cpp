#include <iostream>
#include <vector>
#include <variant>
#include <string>

struct PointI
{
    int x;
    int y;
};

struct PointF
{
    double x;
    double y;
};

struct PointS
{
    double x;
    double y;
    std::string name;
};

namespace var
{
    typedef std::variant<PointI, PointF, PointS> PointValue;
}

namespace uni
{
    enum class PointType {
        Integral,
        Floating,
        String,
    };
    struct PointValue
    {
        PointValue()
            : _type(PointType::Integral)
        {
        }
        PointValue(PointI pi)
            : _type(PointType::Integral)
        {
            _value._point_i = pi;
        }

        PointValue(PointF pf)
            : _type(PointType::Floating)
        {
            _value._point_f = pf;
        }
        PointValue(PointS ps)
            : _type(PointType::Floating)
        {
            new (&_value._point_s) PointS(std::move(ps));
        }

        PointValue(const PointValue& pv)
            : _type(pv._type)
        {
            if (_type == PointType::String)
            {
                new (&_value._point_s) PointS(pv._value._point_s);
            }
        }
        PointValue(PointValue&& pv)
            : _type(pv._type)
        {
            if (_type == PointType::String)
            {
                new (&_value._point_s) PointS(std::move(pv._value._point_s));
            }
        }

        PointValue& operator=(const PointValue& rhs)
        {
            if (this != &rhs)
            {
                if (_type == PointType::String)
                {
                    if (rhs._type != PointType::String)
                    {
                        std::destroy_at(&_value._point_s);
                    }
                }
                else
                {
                    if (rhs._type == PointType::String)
                    {
                        new (&_value._point_s) PointS;
                    }
                }
                _type = rhs._type;
                switch (_type) {
                    case PointType::Integral: _value._point_i = rhs._value._point_i; break;
                    case PointType::Floating: _value._point_f = rhs._value._point_f; break;
                    case PointType::String: _value._point_s = rhs._value._point_s; break;
                }
            }
            return *this;
        }

        PointValue& operator=(PointValue&& rhs)
        {
            if (this != &rhs)
            {
                if (_type == PointType::String)
                {
                    if (rhs._type != PointType::String)
                    {
                        std::destroy_at(&_value._point_s);
                    }
                }
                else
                {
                    if (rhs._type == PointType::String)
                    {
                        new (&_value._point_s) PointS;
                    }
                }
                _type = rhs._type;
                switch (_type) {
                    case PointType::Integral: _value._point_i = rhs._value._point_i; break;
                    case PointType::Floating: _value._point_f = rhs._value._point_f; break;
                    case PointType::String: _value._point_s = std::move(rhs._value._point_s); break;
                }
            }
            return *this;
        }

        ~PointValue()
        {
            if (_type == PointType::String)
            {
                std::destroy_at(&_value._point_s);
            }
        }

        PointType _type;
        union  Storage {
            Storage() {}
            ~Storage()
            {
            }

            PointI _point_i;
            PointF _point_f;
            PointS _point_s;
        } _value;
    };
}

const auto N = 10000000;

template <typename C>
void create(PointI vi, PointF vf, PointS vs, C& container)
{
    for (auto i = 0; i < N; ++i)
    {
        container.push_back(vi);
        container.push_back(vf);
        container.push_back(vs);
    }
}

double sum(const std::vector<var::PointValue>& container)
{
    double s = 0;
    for (auto& v : container)
    {
        switch (v.index())
        {
            case 0:
                s += std::get<0>(v).x;
                break;
            case 1:
                s += std::get<1>(v).x;
                break;
            case 2:
                s += std::get<2>(v).x;
                break;
        }
    }
    return s;
}

double sum_visit(const std::vector<var::PointValue>& container)
{
    double s = 0;
    for (auto& v : container)
    {
        std::visit([&s](auto p) {
                s += p.x;
        }, v);
    }
    return s;
}


double sum(const std::vector<uni::PointValue>& container)
{
    double s = 0;
    for (auto& v : container)
    {
        switch (v._type)
        {
            case uni::PointType::Integral:
                s += v._value._point_i.x;
                break;

            case uni::PointType::Floating:
                s += v._value._point_f.x;
                break;
            case uni::PointType::String:
                s += v._value._point_s.x;
                break;
        }
    }
    return s;
}

int main(int argc, const char* argv[])
{
    std::cout << sizeof(var::PointValue) << std::endl;
    std::cout << sizeof(uni::PointValue) << std::endl;
    if (argc > 1)
    {
        std::vector<var::PointValue> v;
        /* v.reserve(3*N); */
        create(PointI{22, 42}, PointF{3.14, 2.89}, PointS{9.81, 2.53, "very long string needs allocation"}, v);
        std::cout << sum(v) << std::endl;
        /* std::cout << sum_visit(v) << std::endl; */
        std::cout << std::hex << v.data() << ' ' << v.size() << std::endl;
    }
    else
    {
        std::vector<uni::PointValue> v;
        /* v.reserve(3*N); */
        create(PointI{22, 42}, PointF{3.14, 2.89}, PointS{9.81, 2.53, "very long string needs allocation"}, v);
        std::cout << sum(v) << std::endl;
        std::cout << std::hex << v.data() << ' ' << v.size() << std::endl;
    
    }
    return 0;
}
