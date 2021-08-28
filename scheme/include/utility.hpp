#pragma once

#include "definitions.h"

namespace DCPE
{
	using namespace std;

	/**
	 * @brief generate an array of bytes pseudorandomly
	 *
	 * \note
	 * It uses OpenSSL PRG unless TESTING macro is defined.
	 * If it is, C++ standard rand() is used (easy for testing and debugging).
	 *
	 * @param size the number of bytes to generate
	 * @return bytes the resulting bytes
	 */
	bytes get_random_bytes(const number size);

	/**
	 * @brief returns a pseudorandom number
	 *
	 * @param max the non-inclusive max of the range (min is inclusive 0).
	 * @return number the resulting number
	 */
	number get_ramdom_number(const number max);

	/**
	 * @brief Samples a real uniform value [min, max] using random coins from the seed
	 *
	 * @param min the left endpoint of the distribution (inclusive)
	 * @param max the right endpoint of the distribution (inclusive)
	 * @param seed the seed value to supply to pseudo-random generator for coins
	 * @return double the sample value
	 */
	double sample_uniform(const double min, const double max, const number seed);

	/**
	 * @brief Samples a series of normal values using random coins from the seed
	 *
	 * @param mean the mu parameter of the distribution
	 * @param variance the sigma squared parameter of the distribution
	 * @param seed the seed value to supply to pseudo-random generator for coins
	 * @param count the number of samples to return
	 * @return vector<double> the sample values
	 */
	vector<double> sample_normal_series(const double mean, const double variance, const number seed, const number count);

	/**
	 * @brief Samples a value from a Multivariate Normal distribution with variance identity matrix.
	 *
	 * \note
	 * Because the variance is identity, the algorithm is essentially the same as sample_normal_series.
	 *
	 * @param mean the mu parameter of the distribution
	 * @param dimensions the number of dimensions in the distribution
	 * @param seed the seed value to supply to pseudo-random generator for coins
	 * @return vector<double> the sample value
	 */
	vector<double> sample_normal_multivariate_identity(const double mean, const number dimensions, const number seed);
}
