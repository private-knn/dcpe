#include "definitions.h"
#include "utility.hpp"

#include "gtest/gtest.h"
#include <cmath>
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
		int seed	 = TEST_SEED + 1;

		srand(seed);

		auto first = get_random_bytes(n);

		srand(seed);

		auto second = get_random_bytes(n);

		ASSERT_EQ(first, second);
	}

	TEST_F(UtilityTest, RandomNumberSameSeed)
	{
		const auto max = 100000uLL;
		int seed	   = TEST_SEED + 1;

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
			auto first	= sample_normal_multivariate_identity(1, count, i);
			auto second = sample_normal_multivariate_identity(1, count, i);

			ASSERT_EQ(first, second);
		}
	}

	TEST_F(UtilityTest, DISABLED_HmacKeygenGivenHashKey)
	{
		bytes hash_key;
		auto key_size = EVP_MD_size(EVP_get_digestbyname("SHA256"));
		for (size_t i = 0; i < key_size; i++)
		{
			hash_key.push_back((byte)i);
		}

		auto keys = hmac_256_keygen(hash_key);
		ASSERT_NE(EVP_PKEY_cmp(keys.first, keys.second), 1);
	}

	TEST_F(UtilityTest, DISABLED_HmacKeygenTwoKeys)
	{
		auto keys = hmac_256_keygen();

		ASSERT_NE(EVP_PKEY_cmp(keys.first, keys.second), 1);
	}

	TEST_F(UtilityTest, HmacKeygenSame)
	{
		int seed = TEST_SEED + 1;

		srand(seed);

		auto first = hmac_256_keygen();

		srand(seed);

		auto second = hmac_256_keygen();

		ASSERT_NE(EVP_PKEY_cmp_parameters(first.first, second.first), 0);
		ASSERT_NE(EVP_PKEY_cmp_parameters(first.second, second.second), 0);
	}

	TEST_F(UtilityTest, HmacKeygenDifferent)
	{
		auto first	= hmac_256_keygen();
		auto second = hmac_256_keygen();

		ASSERT_NE(EVP_PKEY_cmp(first.first, second.first), 1);
		ASSERT_NE(EVP_PKEY_cmp(first.second, second.second), 1);
	}

	TEST_F(UtilityTest, HmacSignVerify)
	{
		const auto runs = 100;

		for (auto i = 0; i < runs; i++)
		{
			auto message   = get_random_bytes(100);
			auto keys	   = hmac_256_keygen();
			auto signature = hmac_256_sign(keys.first, message);
			auto verified  = hmac_256_verify(keys.second, message, signature);

			ASSERT_TRUE(verified);

			auto different_message = message;
			different_message[0]++;

			verified = hmac_256_verify(keys.second, different_message, signature);
			ASSERT_FALSE(verified);

			auto different_signature = signature;
			different_signature[0]++;

			verified = hmac_256_verify(keys.second, message, different_signature);
			ASSERT_FALSE(verified);
		}
	}

	TEST_F(UtilityTest, DistanceSimple)
	{
		auto result = distance({1, 0, 5}, {0, 2, 4});

		ASSERT_EQ(sqrt(6), result);
	}

	TEST_F(UtilityTest, DistanceVectorsDifferentSize)
	{
		EXPECT_THROW({ distance({1, 0, 5}, {0, 2, 4, 5}); }, Exception);
	}
}

int main(int argc, char **argv)
{
	srand(TEST_SEED);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
