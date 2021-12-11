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

namespace DCPE
{
	/**
	 * @brief can be changed to boost::mt19937 or boost::ecuyer1988.
	 * boost::minstd_rand is NOT crypto-safe.
	 *
	 */
	typedef boost::random::mt19937_64 base_generator_type;

	ull get_ramdom_ull(const ull max)
	{
		ull material[1];
#if defined(TESTING) || defined(DEBUG)
		auto int_material = (int *)material;
		int_material[0]	  = rand();
		int_material[1]	  = rand();
#else
		RAND_bytes((uchar *)material, sizeof(ull));
#endif
		return material[0] % max;
	}

	template <typename VALUE_T>
	VALUE_T sample_uniform(const VALUE_T min, const VALUE_T max, const ull seed)
	{
		base_generator_type generator(seed);

		boost::uniform_real<> distribution(min, max);
		boost::variate_generator<base_generator_type &, boost::uniform_real<>> sampler(generator, distribution);

		return sampler();
	}
	template float sample_uniform<float>(const float min, const float max, const ull seed);
	template double sample_uniform<double>(const double min, const double max, const ull seed);

	template <typename VALUE_T>
	std::vector<VALUE_T> sample_normal_series(const VALUE_T mean, const VALUE_T variance, const ull seed, const int count)
	{
		base_generator_type generator(seed);

		boost::normal_distribution<> distribution(mean, variance);
		boost::variate_generator<base_generator_type &, boost::normal_distribution<>> sampler(generator, distribution);

		std::vector<VALUE_T> samples;
		samples.resize(count);

		for (auto i = 0; i < count; i++)
		{
			samples[i] = sampler();
		}

		return samples;
	}
	template std::vector<float> sample_normal_series<float>(const float mean, const float variance, const ull seed, const int count);
	template std::vector<double> sample_normal_series<double>(const double mean, const double variance, const ull seed, const int count);

	template <typename VALUE_T>
	std::vector<VALUE_T> sample_normal_multivariate_identity(const VALUE_T mean, const int dimensions, const ull seed)
	{
		auto samples = sample_normal_series<VALUE_T>(0.0, 1.0, seed, dimensions);

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
	template std::vector<float> sample_normal_multivariate_identity<float>(const float mean, const int dimensions, const ull seed);
	template std::vector<double> sample_normal_multivariate_identity<double>(const double mean, const int dimensions, const ull seed);

	template <typename VALUE_T>
	VALUE_T distance(std::vector<VALUE_T> first, std::vector<VALUE_T> second)
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
	template float distance(std::vector<float> first, std::vector<float> second);
	template double distance(std::vector<double> first, std::vector<double> second);
}
