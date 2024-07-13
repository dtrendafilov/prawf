#include "precompiled.hxx"

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
            : _type(PointType::String)
        {
            new (&_value._point_s) PointS(std::move(ps));
        }
        
        /*
        PointValue(const PointS& ps)
            : _type(PointType::Floating)
        {
            new (&_value._point_s) PointS(std::move(ps));
        }
        */

        PointValue(const PointValue& pv)
            : _type(pv._type)
        {
            switch (_type) {
                case PointType::Integral: _value._point_i = pv._value._point_i; break;
                case PointType::Floating: _value._point_f = pv._value._point_f; break;
                case PointType::String: new (&_value._point_s) PointS(pv._value._point_s); break;
            }
        }
        PointValue(PointValue&& pv)
            : _type(pv._type)
        {
            switch (_type) {
                case PointType::Integral: _value._point_i = pv._value._point_i; break;
                case PointType::Floating: _value._point_f = pv._value._point_f; break;
                case PointType::String: new (&_value._point_s) PointS(std::move(pv._value._point_s)); break;
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

template <typename C>
void create(int64_t count, PointI vi, PointF vf, PointS vs, C& container)
{
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(vi);
        container.push_back(vf);
        container.push_back(vs);
    }
}

template <typename C>
void create_sorted(int64_t count, PointI vi, PointF vf, PointS vs, C& container)
{
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(vi);
    }
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(vf);
    }
    for (auto i = 0; i < count; ++i)
    {
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
    auto visitor = [&s](auto& p) {
        s += p.x;
    };
    for (auto& v : container)
    {
        std::visit(visitor, v);
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


template <typename T, bool sorted=false>
struct SumFixture : celero::TestFixture
{

    std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> getExperimentValues() const override
	{
		std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> problemSpace;

		// ExperimentValues is part of the base class and allows us to specify
		// some values to control various test runs to end up building a nice graph.
		for(int64_t elements = 1024; elements <= int64_t(65536); elements *= 2)
		{
			problemSpace.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(elements));
		}

		return problemSpace;
	}

    void setUp(const celero::TestFixture::ExperimentValue* experiment) override
    {
        _values.reserve(experiment->Value * 3);
        if (sorted)
        {
            create_sorted(experiment->Value,
                    PointI{22, 42},
                    PointF{3.14, 2.89},
                    PointS{9.81, 2.53, "very long string needs allocation"},
                    _values);
        }
        else
        {
            create(experiment->Value,
                    PointI{22, 42},
                    PointF{3.14, 2.89},
                    PointS{9.81, 2.53, "very long string needs allocation"},
                    _values);
        }
    }

    void tearDown() override
    {
        _values.clear();
    }
    std::vector<T> _values;
};

template <typename T>
struct CreateFixture : celero::TestFixture
{

    std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> getExperimentValues() const override
	{
		std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> problemSpace;

		// ExperimentValues is part of the base class and allows us to specify
		// some values to control various test runs to end up building a nice graph.
		for(int64_t elements = 1024; elements <= int64_t(4096); elements *= 2)
		{
			problemSpace.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(elements));
		}

		return problemSpace;
	}

    void onExperimentStart(const celero::TestFixture::ExperimentValue* experiment) override
    {
        /* _values.reserve(experiment->Value * 3); */
        create(experiment->Value,
                PointI{22, 42},
                PointF{3.14, 2.89},
                PointS{9.81, 2.53, "very long string needs allocation"},
                _values);
    }

    void onExperimentEnd() override
    {
        _values.clear();
    }
    std::vector<T> _values;
};

template <typename T>
using SortedSumFixture = SumFixture<T, true>;

BASELINE_F(Sum, TaggedUnion, SumFixture<uni::PointValue>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(sum(_values));
}

BENCHMARK_F(Sum, Variant, SumFixture<var::PointValue>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(sum(_values));
}

BENCHMARK_F(Sum, Visit, SumFixture<var::PointValue>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(sum_visit(_values));
}

BENCHMARK_F(Sum, SortedUnion, SortedSumFixture<uni::PointValue>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(sum(_values));
}

BENCHMARK_F(Sum, SortedVariant, SortedSumFixture<var::PointValue>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(sum(_values));
}

BENCHMARK_F(Sum, SortedVisit, SortedSumFixture<var::PointValue>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(sum_visit(_values));
}


BASELINE_F(Create, TaggedUnion, CreateFixture<uni::PointValue>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

BENCHMARK_F(Create, Variant, CreateFixture<var::PointValue>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

