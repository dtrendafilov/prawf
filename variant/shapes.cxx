#include "precompiled.hxx"


namespace shapes
{
struct Point
{
    int x;
    int y;

    void scale(int factor)
    {
        x *= factor;
        y *= factor;
    }
};


struct Rectangle
{
    Point TopLeft;
    Point BottomRight;

    void scale(int factor)
    {
        TopLeft.scale(factor);
        BottomRight.scale(factor);
    }
};

struct Triangle
{
    Point A;
    Point B;
    Point C;

    void scale(int factor)
    {
        A.scale(factor);
        B.scale(factor);
        C.scale(factor);
    }
};

struct Circle
{
    Point C;
    int R;

    void scale(int factor)
    {
        C.scale(factor);
        R *= factor;
    }
};

struct ConvexPolygon
{
    std::vector<Point> Points;

    void scale(int factor)
    {
        for (auto& point: Points)
        {
            point.scale(factor);
        }
    }
};

namespace var
{
    typedef std::variant<Triangle, Rectangle, Circle, ConvexPolygon> Shape;
}

namespace uni
{
    enum class ShapeType {
        Triangle,
        Rectangle,
        Circle,
        ConvexPolygon,
    };
    struct Shape
    {
        Shape()
            : _type(ShapeType::Triangle)
        {
        }

        Shape(const Triangle& triangle)
            : _type(ShapeType::Triangle)
        {
            _value._triangle = triangle;
        }

        Shape(const Circle& circle)
            : _type(ShapeType::Circle)
        {
            _value._circle = circle;
        }

        Shape(const Rectangle& rectangle)
            : _type(ShapeType::Rectangle)
        {
            _value._rectangle = rectangle;
        }
        Shape(const ConvexPolygon& polygon)
            : _type(ShapeType::ConvexPolygon)
        {
            new (&_value._polygon) ConvexPolygon(polygon);
        }


        Shape(const Shape& rhs)
            : _type(rhs._type)
        {
            switch (_type)
            {
                case ShapeType::Triangle:
                    _value._triangle = rhs._value._triangle;
                    break;
                case ShapeType::Rectangle:
                    _value._rectangle = rhs._value._rectangle;
                    break;
                case ShapeType::Circle:
                    _value._circle = rhs._value._circle;
                    break;
                case ShapeType::ConvexPolygon:
                    new (&_value._polygon) ConvexPolygon(rhs._value._polygon);
                    break;
            }
        }

        Shape(Shape&& rhs)
            : _type(rhs._type)
        {
            switch (_type)
            {
                case ShapeType::Triangle:
                    _value._triangle = rhs._value._triangle;
                    break;
                case ShapeType::Rectangle:
                    _value._rectangle = rhs._value._rectangle;
                    break;
                case ShapeType::Circle:
                    _value._circle = rhs._value._circle;
                    break;
                case ShapeType::ConvexPolygon:
                    new (&_value._polygon) ConvexPolygon(std::move(rhs._value._polygon));
                    break;
            }
        }

        Shape& operator=(const Shape& rhs)
        {
            if (this != &rhs)
            {
                if (_type == ShapeType::ConvexPolygon)
                {
                    if (rhs._type != ShapeType::ConvexPolygon)
                    {
                        std::destroy_at(&_value._polygon);
                    }
                }
                else
                {
                    if (rhs._type == ShapeType::ConvexPolygon)
                    {
                        new (&_value._polygon) ConvexPolygon;
                    }
                }
                _type = rhs._type;
                switch (_type) {
                    case ShapeType::Triangle: _value._triangle = rhs._value._triangle; break;
                    case ShapeType::Rectangle: _value._rectangle = rhs._value._rectangle; break;
                    case ShapeType::Circle: _value._circle = rhs._value._circle; break;
                    case ShapeType::ConvexPolygon: _value._polygon = rhs._value._polygon; break;
                }
            }
            return *this;
        }

        Shape& operator=(Shape&& rhs)
        {
            if (this != &rhs)
            {
                if (_type == ShapeType::ConvexPolygon)
                {
                    if (rhs._type != ShapeType::ConvexPolygon)
                    {
                        std::destroy_at(&_value._polygon);
                    }
                }
                else
                {
                    if (rhs._type == ShapeType::ConvexPolygon)
                    {
                        new (&_value._polygon) ConvexPolygon;
                    }
                }
                _type = rhs._type;
                switch (_type) {
                    case ShapeType::Triangle: _value._triangle = rhs._value._triangle; break;
                    case ShapeType::Rectangle: _value._rectangle = rhs._value._rectangle; break;
                    case ShapeType::Circle: _value._circle = rhs._value._circle; break;
                    case ShapeType::ConvexPolygon: _value._polygon = std::move(rhs._value._polygon); break;
                }
            }
            return *this;
        }

        ~Shape()
        {
            if (_type == ShapeType::ConvexPolygon)
            {
                std::destroy_at(&_value._polygon);
            }
        }

        ShapeType _type;
        union  Storage {
            Storage() {}
            ~Storage()
            {
            }

            Triangle _triangle;
            Rectangle _rectangle;
            Circle _circle;
            ConvexPolygon _polygon;
        } _value;
    };
}

template <typename C>
void create(int count,
        const Triangle& t,
        const Rectangle& r,
        const Circle& c,
        const ConvexPolygon& p, C& container)
{
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(t);
        container.push_back(r);
        container.push_back(c);
        container.push_back(p);
    }
}

template <typename C>
void create_sorted(int count,
        const Triangle& t,
        const Rectangle& r,
        const Circle& c,
        const ConvexPolygon& p, C& container)
{
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(t);
    }
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(r);
    }
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(c);
    }
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(p);
    }
}

void scale(std::vector<var::Shape>& container, int factor)
{
    for (auto& v : container)
    {
        switch (v.index())
        {
            case 0:
                std::get<0>(v).scale(factor);
                break;
            case 1:
                std::get<1>(v).scale(factor);
                break;
            case 2:
                std::get<2>(v).scale(factor);
                break;
            case 3:
                std::get<3>(v).scale(factor);
                break;
        }
    }
}

void scale_visit(std::vector<var::Shape>& container, int factor)
{
    auto visitor = [factor](auto& s)
    {
        s.scale(factor);
    };
    for (auto& v : container)
    {
        std::visit(visitor, v);
    }
}


void scale(std::vector<uni::Shape>& container, int factor)
{
    for (auto& v : container)
    {
        switch (v._type)
        {
            case uni::ShapeType::Triangle:
                v._value._triangle.scale(factor);
                break;
            case uni::ShapeType::Rectangle:
                v._value._rectangle.scale(factor);
                break;
            case uni::ShapeType::Circle:
                v._value._circle.scale(factor);
                break;
            case uni::ShapeType::ConvexPolygon:
                v._value._polygon.scale(factor);
                break;
        }
    }
}


template <typename T, bool sorted=false>
struct ScaleFixture : celero::TestFixture
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
        ConvexPolygon polygon{{Point{0, 0},
            Point{24, 0},
            Point{24, 42},
            Point{42, 42},
            Point{66, 66},
            Point{24, 66},
            Point{0, 24},
        }};
        _values.reserve(experiment->Value * 4);
        if (sorted)
        {
            create_sorted(experiment->Value,
                    Triangle{Point{0, 0}, Point{0, 24}, Point{42, 0}},
                    Rectangle{Point{24, 42}, Point{42, 24}},
                    Circle{Point{0, 0}, 42},
                    polygon,
                    _values);
        }
        else
        {
            create(experiment->Value,
                    Triangle{Point{0, 0}, Point{0, 24}, Point{42, 0}},
                    Rectangle{Point{24, 42}, Point{42, 24}},
                    Circle{Point{0, 0}, 42},
                    polygon,
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
		for(int64_t elements = 1024; elements <= int64_t(8192); elements *= 2)
		{
			problemSpace.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(elements));
		}

		return problemSpace;
	}

    void onExperimentStart(const celero::TestFixture::ExperimentValue* experiment) override
    {
        ConvexPolygon polygon{{Point{0, 0},
            Point{24, 0},
            Point{24, 42},
            Point{42, 42},
            Point{66, 66},
            Point{24, 66},
            Point{0, 24},
        }};
        create(experiment->Value,
                Triangle{Point{0, 0}, Point{0, 24}, Point{42, 0}},
                Rectangle{Point{24, 42}, Point{42, 24}},
                Circle{Point{0, 0}, 42},
                polygon,
                _values);
    }

    void onExperimentEnd() override
    {
        _values.clear();
    }
    std::vector<T> _values;
};

template <typename T>
using SortedScaleFixture = ScaleFixture<T, true>;

const int SamplesCount = 32;
const int IterationsCount = 128;

BASELINE_F(Scale, TaggedUnion, ScaleFixture<uni::Shape>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, Variant, ScaleFixture<var::Shape>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, Visit, ScaleFixture<var::Shape>, SamplesCount, IterationsCount)
{
    scale_visit(_values, 2);
}

BENCHMARK_F(Scale, SortedUnion, SortedScaleFixture<uni::Shape>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, SortedVariant, SortedScaleFixture<var::Shape>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, SortedVisit, SortedScaleFixture<var::Shape>, SamplesCount, IterationsCount)
{
    scale_visit(_values, 2);
}


BASELINE_F(CreateShapes, TaggedUnion, CreateFixture<uni::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

BENCHMARK_F(CreateShapes, Variant, CreateFixture<var::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

}
