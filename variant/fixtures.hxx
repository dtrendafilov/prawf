#pragma once

template <typename T>
struct Fixture : celero::TestFixture
{

    std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> getExperimentValues() const override
	{
		std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> problemSpace;

		for(int64_t elements = 1024; elements <= int64_t(16384); elements *= 2)
		{
			problemSpace.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(elements));
		}

		return problemSpace;
	}

    std::vector<T> _values;
};

constexpr int SamplesCount = 64;
constexpr int IterationsCount = 64;


