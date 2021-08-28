#include "utility.hpp"

#include <boost/generator_iterator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <iomanip>
#include <openssl/rand.h>
#include <random>
#include <vector>

namespace DCPE
{
	using namespace std;

	/**
	 * @brief can be changed to boost::mt19937 or boost::ecuyer1988.
	 * boost::minstd_rand is NOT crypto-safe.
	 *
	 */
	typedef boost::random::mt19937 base_generator_type;

	bytes get_random_bytes(const number size)
	{
		uchar material[size];
#if defined(TESTING) || defined(DEBUG)
		for (number i = 0; i < size; i++)
		{
			material[i] = (uchar)rand();
		}
#else
		RAND_bytes(material, size);
#endif
		return bytes(material, material + size);
	}

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

	double sample_uniform(const double min, const double max, const number seed)
	{
		base_generator_type generator(seed);

		boost::uniform_real<> distribution(min, max);
		boost::variate_generator<base_generator_type &, boost::uniform_real<>> sampler(generator, distribution);

		return sampler();
	}

	vector<double> sample_normal_series(const double mean, const double variance, const number seed, const number count)
	{
		base_generator_type generator(seed);

		boost::normal_distribution<> distribution(mean, variance);
		boost::variate_generator<base_generator_type &, boost::normal_distribution<>> sampler(generator, distribution);

		vector<double> samples;
		samples.resize(count);

		for (auto i = 0; i < count; i++)
		{
			samples[i] = sampler();
		}

		return samples;
	}

	vector<double> sample_normal_multivariate_identity(const double mean, const number dimensions, const number seed)
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
}
