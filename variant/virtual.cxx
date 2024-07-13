#include "precompiled.hxx"
#include <memory>
#include <numeric>


namespace virt
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


struct IShape
{
    virtual ~IShape() {}
    virtual void scale(int factor) = 0;
    virtual double perimeter() const = 0;
};

typedef std::unique_ptr<IShape> ShapePtr;


struct Rectangle : IShape
{
    Point TopLeft;
    Point BottomRight;

    Rectangle(const Point& topLeft, const Point& bottomRight)
        : TopLeft(topLeft)
        , BottomRight(bottomRight)
    {
    }

    void scale(int factor) override
    {
        TopLeft.scale(factor);
        BottomRight.scale(factor);
    }

    double perimeter() const override
    {
        return 2 * (std::abs(BottomRight.x - TopLeft.x)
                + std::abs(TopLeft.y - BottomRight.y));
    }
};

struct Triangle : IShape
{
    Point A;
    Point B;
    Point C;

    Triangle(const Point& a, const Point& b, const Point& c)
        : A(a)
        , B(b)
        , C(c)
    {}

    void scale(int factor) override
    {
        A.scale(factor);
        B.scale(factor);
        C.scale(factor);
    }

    double perimeter() const override
    {
        return distance(A, B) + distance(B, C) + distance(C, A);
    }
};

struct Circle : IShape
{
    Point C;
    int R;

    Circle(const Point& c, int r)
        : C(c)
        , R(r)
    {}

    void scale(int factor) override
    {
        C.scale(factor);
        R *= factor;
    }

    double perimeter() const override
    {
        return 2 * 3.1415 * R;
    }
};

struct ConvexPolygon : IShape
{
    std::vector<Point> Points;

    ConvexPolygon(const std::vector<Point>& points)
        : Points(points)
    {}

    void scale(int factor) override
    {
        for (auto& point: Points)
        {
            point.scale(factor);
        }
    }

    double perimeter() const override
    {
        auto second = begin(Points) + 1;
        return std::inner_product(second, end(Points), begin(Points),
                distance(Points.front(), Points.back()),
                std::plus<double>{}, distance);
    }
};


template <typename C>
void create(int count,
        const Triangle& t,
        const Rectangle& r,
        const Circle& c,
        const ConvexPolygon& p, C& container)
{
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(std::make_unique<Triangle>(t));
        container.push_back(std::make_unique<Rectangle>(r));
        container.push_back(std::make_unique<Circle>(c));
        container.push_back(std::make_unique<ConvexPolygon>(p));
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
        container.push_back(std::make_unique<Triangle>(t));
    }
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(std::make_unique<Rectangle>(r));
    }
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(std::make_unique<Circle>(c));
    }
    for (auto i = 0; i < count; ++i)
    {
        container.push_back(std::make_unique<ConvexPolygon>(p));
    }
}

void scale(std::vector<ShapePtr>& container, int factor)
{
    for (auto& v : container)
    {
        v->scale(factor);
    }
}

double perimter(std::vector<ShapePtr>& container)
{
    double p = 0;
    for (auto& v : container)
    {
        p += v->perimeter();
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


BENCHMARK_F(Scale, Virtual, OperationFixture<ShapePtr>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, SortedVirtual, SortedFixture<ShapePtr>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Perimeter, Virtual, OperationFixture<ShapePtr>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(perimter(_values));
}

BENCHMARK_F(Perimeter, SortedVirtual, SortedFixture<ShapePtr>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(perimter(_values));
}

BENCHMARK_F(CreateShapes, SortedVariant, CreateFixture<ShapePtr>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

}
