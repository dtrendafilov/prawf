#pragma once

template <typename T>
struct Fixture : celero::TestFixture
{

    std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> getExperimentValues() const override
	{
		std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> problemSpace;

#if defined(VARIANT_SMALL_TESTS)
        int64_t start = 256;
        int64_t limit = 512;
#elif defined(NDEBUG)
        int64_t start = 1024;
        int64_t limit = 4096;
#else
        int64_t start = 256;
        int64_t limit = 1025;
#endif
		for(int64_t elements = start; elements <= limit; elements *= 2)
		{
			problemSpace.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(elements));
		}

		return problemSpace;
	}

    std::vector<T> _values;
};

constexpr int SamplesCount = 64;
constexpr int IterationsCount = 64;


