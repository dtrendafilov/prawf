#include "precompiled.hxx"
#include <numeric>


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

    double distance_to(const Point& to) const
    {
        const auto dx = to.x - x;
        const auto dy = to.y - y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

double distance(const Point& lhs, const Point& rhs)
{
    return lhs.distance_to(rhs);
}


struct Rectangle
{
    Point TopLeft;
    Point BottomRight;

    void scale(int factor)
    {
        TopLeft.scale(factor);
        BottomRight.scale(factor);
    }

    double perimeter() const
    {
        return 2 * (std::abs(BottomRight.x - TopLeft.x)
                + std::abs(TopLeft.y - BottomRight.y));
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

    double perimeter() const
    {
        return distance(A, B) + distance(B, C) + distance(C, A);
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

    double perimeter() const
    {
        return 2 * 3.1415 * R;
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

    double perimeter() const
    {
        auto second = begin(Points) + 1;
        return std::inner_product(second, end(Points), begin(Points),
                distance(Points.front(), Points.back()),
                std::plus<double>{}, distance);
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
void create(int64_t count,
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
void create_sorted(int64_t count,
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

double perimeter(std::vector<var::Shape>& container)
{
    double p = 0;
    for (auto& v : container)
    {
        switch (v.index())
        {
            case 0:
                p += std::get<0>(v).perimeter();
                break;
            case 1:
                p += std::get<1>(v).perimeter();
                break;
            case 2:
                p += std::get<2>(v).perimeter();
                break;
            case 3:
                p += std::get<3>(v).perimeter();
                break;
        }
    }
    return p;
}

double perimeter_visit(std::vector<var::Shape>& container)
{
    double p = 0;
    auto visitor = [&p](auto& s)
    {
        p += s.perimeter();
    };
    for (auto& v : container)
    {
        std::visit(visitor, v);
    }
    return p;
}


double perimeter(std::vector<uni::Shape>& container)
{
    double p = 0;
    for (auto& v : container)
    {
        switch (v._type)
        {
            case uni::ShapeType::Triangle:
                p += v._value._triangle.perimeter();
                break;
            case uni::ShapeType::Rectangle:
                p += v._value._rectangle.perimeter();
                break;
            case uni::ShapeType::Circle:
                p += v._value._circle.perimeter();
                break;
            case uni::ShapeType::ConvexPolygon:
                p += v._value._polygon.perimeter();
                break;
        }
    }
    return p;
}

Rectangle bbox(const uni::Shape& lhs, const uni::Shape& rhs)
{
    switch (lhs._type)
    {
        case uni::ShapeType::Triangle:
        {
            switch (rhs._type)
            {
                case uni::ShapeType::Triangle: return Rectangle{lhs._value._triangle.A, rhs._value._triangle.C};
                case uni::ShapeType::Rectangle: return Rectangle{lhs._value._triangle.A, rhs._value._rectangle.BottomRight};
                case uni::ShapeType::Circle: return Rectangle{lhs._value._triangle.A, rhs._value._circle.C};
                case uni::ShapeType::ConvexPolygon: return Rectangle{lhs._value._triangle.A, rhs._value._polygon.Points.back()};
            }
        }
        case uni::ShapeType::Rectangle:
        {
            switch (rhs._type)
            {
                case uni::ShapeType::Triangle: return Rectangle{lhs._value._rectangle.TopLeft, rhs._value._triangle.C};
                case uni::ShapeType::Rectangle: return Rectangle{lhs._value._rectangle.TopLeft, rhs._value._rectangle.BottomRight};
                case uni::ShapeType::Circle: return Rectangle{lhs._value._rectangle.TopLeft, rhs._value._circle.C};
                case uni::ShapeType::ConvexPolygon: return Rectangle{lhs._value._rectangle.TopLeft, rhs._value._polygon.Points.back()};
            }
        }
        case uni::ShapeType::Circle:
        {
            switch (rhs._type)
            {
                case uni::ShapeType::Triangle: return Rectangle{lhs._value._circle.C, rhs._value._triangle.C};
                case uni::ShapeType::Rectangle: return Rectangle{lhs._value._circle.C, rhs._value._rectangle.BottomRight};
                case uni::ShapeType::Circle: return Rectangle{lhs._value._circle.C, rhs._value._circle.C};
                case uni::ShapeType::ConvexPolygon: return Rectangle{lhs._value._circle.C, rhs._value._polygon.Points.back()};
            }
        }
        case uni::ShapeType::ConvexPolygon:
        {
            switch (rhs._type)
            {
                case uni::ShapeType::Triangle: return Rectangle{lhs._value._polygon.Points.front(), rhs._value._triangle.C};
                case uni::ShapeType::Rectangle: return Rectangle{lhs._value._polygon.Points.front(), rhs._value._rectangle.BottomRight};
                case uni::ShapeType::Circle: return Rectangle{lhs._value._polygon.Points.front(), rhs._value._circle.C};
                case uni::ShapeType::ConvexPolygon: return Rectangle{lhs._value._polygon.Points.front(), rhs._value._polygon.Points.back()};
            }
        }
    }
}

double bbox(std::vector<uni::Shape>& container)
{
    double p = 0;

    for (auto& l : container)
    {
        for (auto& r : container)
        {
            p += bbox(l, r).perimeter();
        }
    }
    return p;
}

struct MakeBBox
{
    Rectangle operator()(const Triangle& lhs, const Triangle& rhs) const
    {
        return Rectangle{lhs.A, rhs.C};
    }
    Rectangle operator()(const Triangle& lhs, const Rectangle& rhs) const
    {
        return Rectangle{lhs.A, rhs.BottomRight};
    }
    Rectangle operator()(const Triangle& lhs, const Circle& rhs) const
    {
        return Rectangle{lhs.A, rhs.C};
    }

    Rectangle operator()(const Triangle& lhs, const ConvexPolygon& rhs) const
    {
        return Rectangle{lhs.A, rhs.Points.back()};
    }

    Rectangle operator()(const Rectangle& lhs, const Triangle& rhs) const
    {
        return Rectangle {lhs.TopLeft, rhs.C};
    }

    Rectangle operator()(const Rectangle& lhs, const Rectangle& rhs) const
    {
        return Rectangle {lhs.TopLeft, rhs.BottomRight};
    }
    Rectangle operator()(const Rectangle& lhs, const Circle& rhs) const
    {
        return Rectangle {lhs.TopLeft, rhs.C};
    }
    Rectangle operator()(const Rectangle& lhs, const ConvexPolygon& rhs) const
    {
        return Rectangle {lhs.TopLeft, rhs.Points.back()};
    }

    Rectangle operator()(const Circle& lhs, const Triangle& rhs) const
    {
        return Rectangle {lhs.C, rhs.C};
    }
    Rectangle operator()(const Circle& lhs, const Rectangle& rhs) const
    {
        return Rectangle {lhs.C, rhs.BottomRight};
    }
    Rectangle operator()(const Circle& lhs, const Circle& rhs) const
    {
        return Rectangle {lhs.C, rhs.C};
    }
    Rectangle operator()(const Circle& lhs, const ConvexPolygon& rhs) const
    {
        return Rectangle {lhs.C, rhs.Points.back()};
    }

    Rectangle operator()(const ConvexPolygon& lhs, const Triangle& rhs) const
    {
        return Rectangle {lhs.Points.front(), rhs.C};
    }
    Rectangle operator()(const ConvexPolygon& lhs, const Rectangle& rhs) const
    {
        return Rectangle {lhs.Points.front(), rhs.BottomRight};
    }
    Rectangle operator()(const ConvexPolygon& lhs, const Circle& rhs) const
    {
        return Rectangle {lhs.Points.front(), rhs.C};
    }
    Rectangle operator()(const ConvexPolygon& lhs, const ConvexPolygon& rhs) const
    {
        return Rectangle {lhs.Points.front(), rhs.Points.back()};
    }
};

double bbox_visit(std::vector<var::Shape>& container)
{
    double p = 0;
    for (auto& l : container)
    {
        for (auto& r : container)
        {
            p += std::visit(MakeBBox{}, l, r).perimeter();
        }
    }
    return p;
}

template <typename T>
struct OperationFixture : Fixture<T>
{
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
        this->_values.reserve(experiment->Value * 4);
        create(experiment->Value,
                Triangle{Point{0, 0}, Point{0, 24}, Point{42, 0}},
                Rectangle{Point{24, 42}, Point{42, 24}},
                Circle{Point{0, 0}, 42},
                polygon,
                this->_values);
    }

    void tearDown() override
    {
        this->_values.clear();
    }
};

template <typename T>
struct SortedFixture : Fixture<T>
{
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
        this->_values.reserve(experiment->Value * 4);
        create_sorted(experiment->Value,
                Triangle{Point{0, 0}, Point{0, 24}, Point{42, 0}},
                Rectangle{Point{24, 42}, Point{42, 24}},
                Circle{Point{0, 0}, 42},
                polygon,
                this->_values);
    }

    void tearDown() override
    {
        this->_values.clear();
    }
};


template <typename T>
struct CreateFixture : Fixture<T>
{
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
        this->_values.reserve(experiment->Value * 4);
        create(experiment->Value,
                Triangle{Point{0, 0}, Point{0, 24}, Point{42, 0}},
                Rectangle{Point{24, 42}, Point{42, 24}},
                Circle{Point{0, 0}, 42},
                polygon,
                this->_values);
    }

    void onExperimentEnd() override
    {
        this->_values.clear();
    }
};

template <typename T>
struct CreateSorted : Fixture<T>
{
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
        this->_values.reserve(experiment->Value * 4);
        create_sorted(experiment->Value,
                Triangle{Point{0, 0}, Point{0, 24}, Point{42, 0}},
                Rectangle{Point{24, 42}, Point{42, 24}},
                Circle{Point{0, 0}, 42},
                polygon,
                this->_values);
    }

    void onExperimentEnd() override
    {
        this->_values.clear();
    }
};




BASELINE_F(Scale, TaggedUnion, OperationFixture<uni::Shape>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, Variant, OperationFixture<var::Shape>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, Visit, OperationFixture<var::Shape>, SamplesCount, IterationsCount)
{
    scale_visit(_values, 2);
}

BENCHMARK_F(Scale, SortedUnion, SortedFixture<uni::Shape>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, SortedVariant, SortedFixture<var::Shape>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, SortedVisit, SortedFixture<var::Shape>, SamplesCount, IterationsCount)
{
    scale_visit(_values, 2);
}

BASELINE_F(Perimeter, TaggedUnion, OperationFixture<uni::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(perimeter(_values));
}

BENCHMARK_F(Perimeter, Variant, OperationFixture<var::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(perimeter(_values));
}

BENCHMARK_F(Perimeter, Visit, OperationFixture<var::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(perimeter_visit(_values));
}

BENCHMARK_F(Perimeter, SortedUnion, SortedFixture<uni::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(perimeter(_values));
}

BENCHMARK_F(Perimeter, SortedVariant, SortedFixture<var::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(perimeter(_values));
}

BENCHMARK_F(Perimeter, SortedVisit, SortedFixture<var::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(perimeter_visit(_values));
}


BASELINE_F(CreateShapes, TaggedUnion, CreateFixture<uni::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

BENCHMARK_F(CreateShapes, Variant, CreateFixture<var::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

BENCHMARK_F(CreateShapes, SortedUnion, CreateSorted<uni::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

BENCHMARK_F(CreateShapes, SortedVariant, CreateSorted<var::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

BASELINE_F(BBox, TaggedUnion, OperationFixture<uni::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(bbox(_values));
}

BENCHMARK_F(BBox, Visit, OperationFixture<var::Shape>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(bbox_visit(_values));
}

}
