#include "utility.hpp"

#include <boost/generator_iterator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <iomanip>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <random>
#include <vector>

#ifndef TESTING
#define CHECK_OPENSSL_ERROR(condition, message)                                        \
	if (condition)                                                                     \
	{                                                                                  \
		throw Exception(boost::format("%s, error 0x%lx") % message % ERR_get_error()); \
	}
#else
#define CHECK_OPENSSL_ERROR(condition, message)
#endif

namespace DCPE
{
	using namespace std;

	/**
	 * @brief can be changed to boost::mt19937 or boost::ecuyer1988.
	 * boost::minstd_rand is NOT crypto-safe.
	 *
	 */
	typedef boost::random::mt19937_64 base_generator_type;

	number get_ramdom_number(const number max)
	{
		number material[1];
#if defined(TESTING) || defined(DEBUG)
		auto int_material = (int *)material;
		int_material[0]	  = rand();
		int_material[1]	  = rand();
#else
		RAND_bytes((uchar *)material, sizeof(number));
#endif
		return material[0] % max;
	}

	number bytes_to_number(bytes::iterator first_byte)
	{
		number material[1];
		auto byte_material = (byte *)material;
		for (auto i = 0; i < sizeof(number) / sizeof(byte); i++)
		{
			byte_material[0] = *first_byte;
			first_byte++;
		}

		return material[0];
	}

	VALUE_T sample_uniform(const VALUE_T min, const VALUE_T max, const number seed)
	{
		base_generator_type generator(seed);

		boost::uniform_real<> distribution(min, max);
		boost::variate_generator<base_generator_type &, boost::uniform_real<>> sampler(generator, distribution);

		return sampler();
	}

	vector<VALUE_T> sample_normal_series(const VALUE_T mean, const VALUE_T variance, const number seed, const number count)
	{
		base_generator_type generator(seed);

		boost::normal_distribution<> distribution(mean, variance);
		boost::variate_generator<base_generator_type &, boost::normal_distribution<>> sampler(generator, distribution);

		vector<VALUE_T> samples;
		samples.resize(count);

		for (auto i = 0; i < count; i++)
		{
			samples[i] = sampler();
		}

		return samples;
	}

	vector<VALUE_T> sample_normal_multivariate_identity(const VALUE_T mean, const number dimensions, const number seed)
	{
		auto samples = sample_normal_series(0.0, 1.0, seed, dimensions);

		// optimization
		if (mean != 0)
		{
			return samples;
		}

		for (auto i = 0; i < dimensions; i++)
		{
			samples[i] += mean;
		}

		return samples;
	}

	VALUE_T distance(vector<VALUE_T> first, vector<VALUE_T> second)
	{
		if (first.size() != second.size())
		{
			throw Exception(boost::format("distance: vectors of different lengths (%d vs %d)") % first.size() % second.size());
		}

		auto result = 0.0;
		for (size_t i = 0; i < first.size(); i++)
		{
			result += (first[i] - second[i]) * (first[i] - second[i]);
		}

		return sqrt(result);
	}
}
