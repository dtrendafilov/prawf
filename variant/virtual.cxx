#include "precompiled.hxx"
#include <memory>


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
};

struct IShape
{
    virtual ~IShape() {}
    virtual void scale(int factor) = 0;
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

BENCHMARK_F(Scale, Virtual, ScaleFixture<ShapePtr>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(Scale, SortedVirtual, SortedScaleFixture<ShapePtr>, SamplesCount, IterationsCount)
{
    scale(_values, 2);
}

BENCHMARK_F(CreateShapes, SortedVariant, CreateFixture<ShapePtr>, SamplesCount, IterationsCount)
{
    celero::DoNotOptimizeAway(_values);
}

}
