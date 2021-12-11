#include "definitions.h"
#include "scheme.hpp"
#include "utility.hpp"

#include "gtest/gtest.h"
#include <cmath>
#include <numeric>

// change to run all tests from different seed
const auto TEST_SEED = 0x13;

namespace DCPE
{
	template <typename VALUE_T>
	class UtilityTest : public ::testing::Test
	{
	};

	using testing::Types;

	typedef Types<float, double> ValidVectorTypes;
	TYPED_TEST_SUITE(UtilityTest, ValidVectorTypes);

	TYPED_TEST(UtilityTest, UniformDifferentSeed)
	{
		const auto seed = 13uLL;
		for (int i = 0; i < 100; i++)
		{
			auto first	= sample_uniform<TypeParam>(0.0, 100.0, seed);
			auto second = sample_uniform<TypeParam>(0.0, 1000.0, seed + 1);

			ASSERT_NE(first, second);
		}
	}

	TYPED_TEST(UtilityTest, UniformSameSeed)
	{
		for (int i = 0; i < 100; i++)
		{
			auto first	= sample_uniform<TypeParam>(0.0, 100.0, i);
			auto second = sample_uniform<TypeParam>(0.0, 100.0, i);

			ASSERT_EQ(first, second);
		}
	}

	TYPED_TEST(UtilityTest, UniformCheckDistribution)
	{
		const auto seed	 = 13uLL;
		const auto min	 = 0.0;
		const auto max	 = 100.0;
		const auto runs	 = 100000uLL;
		const auto error = runs * 0.01;

		std::vector<TypeParam> samples;
		for (int i = 0; i < runs; i++)
		{
			auto sample = sample_uniform<TypeParam>(min, max, seed + i);

			samples.push_back(sample);
			ASSERT_GE(sample, min);
			ASSERT_LE(sample, max);
		}

		TypeParam sum  = std::accumulate(samples.begin(), samples.end(), 0.0);
		TypeParam mean = sum / samples.size();

		ASSERT_NEAR(mean, (max - min) / 2, error);

		TypeParam sq_sum   = std::inner_product(samples.begin(), samples.end(), samples.begin(), 0.0);
		TypeParam variance = sq_sum / samples.size() - mean * mean;

		ASSERT_NEAR(variance, (max - min) * (max - min) / 12.0, error);
	}

	TYPED_TEST(UtilityTest, NormalDifferentSeed)
	{
		const auto seed	 = 13uLL;
		const auto count = 10uLL;
		for (int i = 0; i < 100; i++)
		{
			auto first	= sample_normal_series<TypeParam>(0.0, 100.0, seed, count);
			auto second = sample_normal_series<TypeParam>(0.0, 100.0, seed + 1, count);

			ASSERT_NE(first, second);
		}
	}

	TYPED_TEST(UtilityTest, NormalSameSeed)
	{
		const auto count = 10uLL;
		for (int i = 0; i < 100; i++)
		{
			auto first	= sample_normal_series<TypeParam>(0.0, 100.0, i, count);
			auto second = sample_normal_series<TypeParam>(0.0, 100.0, i, count);

			ASSERT_EQ(first, second);
		}
	}

	TYPED_TEST(UtilityTest, NormalCheckDistribution)
	{
		const auto seed		= 13uLL;
		const auto mu		= 0.0;
		const auto sigma_sq = 10.0;
		const auto runs		= 100000uLL;
		const auto error	= runs * 0.01;

		auto samples = sample_normal_series<TypeParam>(mu, sigma_sq, seed, runs);

		TypeParam sum  = std::accumulate(samples.begin(), samples.end(), 0.0);
		TypeParam mean = sum / samples.size();

		ASSERT_NEAR(mean, mu, error);

		TypeParam sq_sum   = std::inner_product(samples.begin(), samples.end(), samples.begin(), 0.0);
		TypeParam variance = sq_sum / samples.size() - mean * mean;

		ASSERT_NEAR(variance, sigma_sq, error);
	}

	TYPED_TEST(UtilityTest, NormalMultivariateDifferentSeed)
	{
		const auto seed	 = 13uLL;
		const auto count = 10uLL;
		for (int i = 0; i < 100; i++)
		{
			auto first	= sample_normal_multivariate_identity<TypeParam>(0.0, count, seed);
			auto second = sample_normal_multivariate_identity<TypeParam>(0.0, count, seed + 1);

			ASSERT_NE(first, second);
		}
	}

	TYPED_TEST(UtilityTest, NormalMultivariateSameSeed)
	{
		const auto count = 10uLL;
		for (int i = 0; i < 100; i++)
		{
			auto first	= sample_normal_multivariate_identity<TypeParam>(1.0, count, i);
			auto second = sample_normal_multivariate_identity<TypeParam>(1.0, count, i);

			ASSERT_EQ(first, second);
		}
	}

	TYPED_TEST(UtilityTest, DistanceSimple)
	{
		std::vector<TypeParam> a = {1.0, 0.0, 5.0};
		std::vector<TypeParam> b = {0.0, 2.0, 4.0};
		auto result			= distance<TypeParam>(a, b);

		ASSERT_NEAR(sqrt(6), result, 0.0000001);
	}

	TYPED_TEST(UtilityTest, DistanceVectorsDifferentSize)
	{
		std::vector<TypeParam> a = {1.0, 0.0, 5.0};
		std::vector<TypeParam> b = {0.0, 2.0, 4.0, 5.0};
		EXPECT_THROW({ distance<TypeParam>(a, b); }, Exception);
	}
}

int main(int argc, char **argv)
{
	srand(TEST_SEED);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
