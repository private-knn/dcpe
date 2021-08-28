#include "definitions.h"
#include "utility.hpp"

#include "gtest/gtest.h"
#include <numeric>

using namespace std;

namespace DCPE
{
	class UtilityTest : public ::testing::Test
	{
	};

	TEST_F(UtilityTest, RandomNoSeed)
	{
		const auto n = 20uLL;
		for (number i = 0; i < 100; i++)
		{
			auto first	= get_random_bytes(n);
			auto second = get_random_bytes(n);

			ASSERT_NE(first, second);
		}
	}

	TEST_F(UtilityTest, RandomSameSeed)
	{
		const auto n = 20uLL;
		int seed	 = 0x15;

		srand(seed);

		auto first = get_random_bytes(n);

		srand(seed);

		auto second = get_random_bytes(n);

		ASSERT_EQ(first, second);
	}

	TEST_F(UtilityTest, RandomNumberSameSeed)
	{
		const auto max = 100000uLL;
		int seed	   = 0x15;

		srand(seed);

		auto first = get_ramdom_number(max);

		srand(seed);

		auto second = get_ramdom_number(max);

		ASSERT_EQ(first, second);
	}

	TEST_F(UtilityTest, UniformDifferentSeed)
	{
		const auto seed = 13uLL;
		for (number i = 0; i < 100; i++)
		{
			auto first	= sample_uniform(0, 100, seed);
			auto second = sample_uniform(0, 100, seed + 1);

			ASSERT_NE(first, second);
		}
	}

	TEST_F(UtilityTest, UniformSameSeed)
	{
		for (number i = 0; i < 100; i++)
		{
			auto first	= sample_uniform(0, 100, i);
			auto second = sample_uniform(0, 100, i);

			ASSERT_EQ(first, second);
		}
	}

	TEST_F(UtilityTest, UniformCheckDistribution)
	{
		const auto seed	 = 13uLL;
		const auto min	 = 0.0;
		const auto max	 = 100.0;
		const auto runs	 = 100000uLL;
		const auto error = runs * 0.01;

		vector<double> samples;
		for (number i = 0; i < runs; i++)
		{
			auto sample = sample_uniform(min, max, seed + i);

			samples.push_back(sample);
			ASSERT_GE(sample, min);
			ASSERT_LE(sample, max);
		}

		double sum	= std::accumulate(samples.begin(), samples.end(), 0.0);
		double mean = sum / samples.size();

		ASSERT_NEAR(mean, (max - min) / 2, error);

		double sq_sum	= std::inner_product(samples.begin(), samples.end(), samples.begin(), 0.0);
		double variance = sq_sum / samples.size() - mean * mean;

		ASSERT_NEAR(variance, (max - min) * (max - min) / 12.0, error);
	}

	TEST_F(UtilityTest, NormalDifferentSeed)
	{
		const auto seed	 = 13uLL;
		const auto count = 10uLL;
		for (number i = 0; i < 100; i++)
		{
			auto first	= sample_normal_series(0, 100, seed, count);
			auto second = sample_normal_series(0, 100, seed + 1, count);

			ASSERT_NE(first, second);
		}
	}

	TEST_F(UtilityTest, NormalSameSeed)
	{
		const auto count = 10uLL;
		for (number i = 0; i < 100; i++)
		{
			auto first	= sample_normal_series(0, 100, i, count);
			auto second = sample_normal_series(0, 100, i, count);

			ASSERT_EQ(first, second);
		}
	}

	TEST_F(UtilityTest, NormalCheckDistribution)
	{
		const auto seed		= 13uLL;
		const auto mu		= 0.0;
		const auto sigma_sq = 10.0;
		const auto runs		= 100000uLL;
		const auto error	= runs * 0.01;

		auto samples = sample_normal_series(mu, sigma_sq, seed, runs);

		double sum	= std::accumulate(samples.begin(), samples.end(), 0.0);
		double mean = sum / samples.size();

		ASSERT_NEAR(mean, mu, error);

		double sq_sum	= std::inner_product(samples.begin(), samples.end(), samples.begin(), 0.0);
		double variance = sq_sum / samples.size() - mean * mean;

		ASSERT_NEAR(variance, sigma_sq, error);
	}

	TEST_F(UtilityTest, NormalMultivariateDifferentSeed)
	{
		const auto seed	 = 13uLL;
		const auto count = 10uLL;
		for (number i = 0; i < 100; i++)
		{
			auto first	= sample_normal_multivariate_identity(0, count, seed);
			auto second = sample_normal_multivariate_identity(0, count, seed + 1);

			ASSERT_NE(first, second);
		}
	}

	TEST_F(UtilityTest, NormalMultivariateSameSeed)
	{
		const auto count = 10uLL;
		for (number i = 0; i < 100; i++)
		{
			auto first	= sample_normal_multivariate_identity(0, count, i);
			auto second = sample_normal_multivariate_identity(0, count, i);

			ASSERT_EQ(first, second);
		}
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
